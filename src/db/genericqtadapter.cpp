#include "genericqtadapter.h"
#include "qtqueryresult.h"
#include "errorresult.h"
#include "schema.h"

#include <QSqlError>

using namespace Qt::StringLiterals;

static int CONNECTION_COUNT = 0;

GenericQtAdapter::GenericQtAdapter(const QString &dbDriver, const QUrl &url, QObject *parent)
    : DatabaseAdapter(url, parent)
{
    QString name = u"c%1"_s.arg(++CONNECTION_COUNT);

    m_db = QSqlDatabase::addDatabase(dbDriver, name);

    if(!url.host().isEmpty())
        m_db.setHostName(url.host());

    if(url.port() >= 0)
        m_db.setPort(url.port());

    if(!url.userName().isEmpty())
        m_db.setUserName(url.userName());

    if(!url.password().isEmpty())
        m_db.setPassword(url.password());

    QString path = url.path();
    if(path.startsWith(u'/') && dbDriver != u"QSQLITE"_s)
        path.removeFirst();
    if(!path.isEmpty())
        m_db.setDatabaseName(path);

    m_db.open();
}

GenericQtAdapter::~GenericQtAdapter()
{
    const QString name = m_db.connectionName();
    m_db.close();
    QSqlDatabase::removeDatabase(name);
}

bool GenericQtAdapter::isOpen() const
{
    return m_db.isOpen();
}

QString GenericQtAdapter::name() const {
    return m_db.databaseName();
}

QString GenericQtAdapter::databaseError() const
{
    return m_db.lastError().text();
}

QueryResult *GenericQtAdapter::query(const QString &query, int maxRows)
{
    if(!m_db.isOpen()) {
        return new ErrorResult(m_db.lastError().text(), query, this);
    }

    QSqlQuery q{m_db};
    q.setForwardOnly(true);
    if(q.exec(query))
        q.next();

    auto result = new QtQueryResult(std::move(q), this);
    result->fetchMore(maxRows);

    return result;
}

void GenericQtAdapter::refreshSchema()
{
    // Get tables and columns
    QSqlQuery q(m_db);

    if(!q.exec(u"SELECT table_schema, table_name, table_type, column_name, data_type FROM information_schema.columns NATURAL JOIN information_schema.tables ORDER BY table_schema, table_name, ordinal_position ASC"_s)) {
        qWarning() << "Couldn't get database schema" << q.lastError().text();
        return;
    }

    QList<dbschema::Schema> schemas;
    while(q.next()) {
        const QString sname = q.value(0).toString();
        const QString tname = q.value(1).toString();
        const QString ttype = q.value(2).toString();
        const QString cname = q.value(3).toString();
        const QString ctype = q.value(4).toString();

        if(schemas.isEmpty() || schemas.last().name != sname)
            schemas.append(dbschema::Schema{sname, QList<dbschema::Table>()});
        dbschema::Schema &schema = schemas.last();

        if(schema.tables.isEmpty() || schema.tables.last().name != tname) {
            dbschema::Table::Type type = dbschema::Table::Type::Table;
            if(ttype==u"VIEW"_s)
                type = dbschema::Table::Type::View;
            else if(sname == u"pg_catalog"_s)
                type = dbschema::Table::Type::SystemTable;
            schema.tables.append(dbschema::Table{tname, QList<dbschema::Column>(), type});
        }
        dbschema::Table &table = schema.tables.last();

        // Add column
        table.columns.append({
            cname,
            ctype,
            false,
            false,
            false,
            QString(),
            QString(),
            dbschema::ForeignKeyAction::Unknown,
            dbschema::ForeignKeyAction::Unknown
        });
    }

    // Get constraints
    q.exec(QStringLiteral("SELECT"
           " table_schema, table_name, column_name, constraint_type, NULL, NULL, NULL, NULL, NULL, NULL"
           " FROM information_schema.table_constraints"
           " NATURAL JOIN information_schema.constraint_column_usage"
           " WHERE constraint_type!='FOREIGN KEY'"

           " UNION "

           "SELECT"
           " c.table_schema, c.table_name, c.column_name, 'FOREIGN KEY', rk.table_catalog, rk.table_schema, rk.table_name, rk.column_name, update_rule, delete_rule"
           " FROM information_schema.referential_constraints"
           " NATURAL JOIN information_schema.key_column_usage c"
           " JOIN information_schema.key_column_usage rk ON (unique_constraint_schema=rk.constraint_schema AND unique_constraint_name=rk.constraint_name)"

           " ORDER BY table_schema, table_name, column_name ASC"
    ));

    while(q.next()) {
        QString schema = q.value(0).toString();
        QString table = q.value(1).toString();
        QString column = q.value(2).toString();
        QString type = q.value(3).toString();

        // TODO optimize
        for(int i=0;i<schemas.count();++i) {
            dbschema::Schema &s = schemas[i];
            if(s.name == schema) {
                for(int j=0;j<s.tables.length();++j) {
                    dbschema::Table &t = s.tables[j];
                    if(t.name == table) {
                        for(int k=0;k<t.columns.length();++k) {
                            dbschema::Column &c = t.columns[k];
                            if(c.name == column) {
                                if(type == u"FOREIGN KEY"_s) {
                                    // 4 - to database
                                    // 5 - to schema
                                    c.foreignKeyToTable = q.value(6).toString();
                                    c.foreignKeyToColumn = q.value(7).toString();
                                    c.onUpdate = dbschema::foreignKeyActionFromString(q.value(8).toString());
                                    c.onDelete = dbschema::foreignKeyActionFromString(q.value(9).toString());
                                } else if(type==u"PRIMARY KEY"_s) {
                                    c.isPrimaryKey = true;

                                } else if(type==u"UNIQUE"_s) {
                                    c.isUnique = true;
                                }
                            }
                        }
                        break;
                    }
                }
                break;
            }
        }
    }

    m_schema = {
        schemas,
        schemas.length() > 1
    };

    Q_EMIT schemaRefreshed(m_schema);
}

void GenericQtAdapter::refreshOtherDatabases()
{
    QSqlQuery q(m_db);

    if(m_db.driverName() == u"QMYSQL") {
        q.exec(u"SHOW DATABASES"_s);

    } else if(m_db.driverName() == u"QPSQL") {
        q.exec(u"SELECT datname FROM pg_database WHERE datallowconn=true ORDER BY datname ASC"_s);

    } else {
        Q_EMIT otherDatabases(QStringList() << name(), name());
        return;
    }

    QStringList list;
    while(q.next())
        list << q.value(0).toString();
    Q_EMIT otherDatabases(list, name());
}
