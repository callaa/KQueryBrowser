#include "sqliteadapter.h"
#include "schema.h"

#include <QSqlQuery>
#include <KLocalizedString>
#include <QSqlError>

using namespace Qt::StringLiterals;

SqliteAdapter::SqliteAdapter(const QUrl &url, QObject *parent)
    : GenericQtAdapter(u"QSQLITE"_s, url, parent)
{
}

void SqliteAdapter::refreshSchema()
{
    // First, get the list of tables
    QSqlQuery q{m_db};
    q.setForwardOnly(true);
    if(!q.exec(u"PRAGMA table_list"_s)) {
        qWarning() << "Failed to get table list" << q.lastError().text();
        return;
    }

    QList<dbschema::Schema> schemas;

    while(q.next()) {
        const QString schemaName = q.value(0).toString();
        if(schemas.isEmpty() || schemas.last().name != schemaName) {
            schemas.append(dbschema::Schema{schemaName, QList<dbschema::Table>{}});
        }
        auto &schema = schemas.last();
        dbschema::Table::Type tableType = dbschema::Table::Type::Table;

        const QString tableName = q.value(1).toString();

        const QString tableTypeStr = q.value(2).toString();
        if(tableTypeStr == u"view"_s)
            tableType = dbschema::Table::Type::View;
        else if(tableTypeStr == u"virtual"_s)
            tableType = dbschema::Table::Type::Virtual;

        schema.tables.append({tableName, QList<dbschema::Column>{}, tableType});
        auto &table = schema.tables.last();

        // Query columns for each table
        QSqlQuery cq{m_db};
        cq.setForwardOnly(true);
        if(!cq.exec(u"PRAGMA %1.table_info(\"%2\")"_s.arg(schema.name, table.name))) {
            qWarning() << "Failed to get table info for" << table.name << q.lastError().text();
            return;
        }

        while(cq.next()) {
            table.columns.append({
                cq.value(1).toString(),
                cq.value(2).toString(),
                cq.value(5).toBool(),
                false,
                cq.value(3).toBool(),
                QString(), // foreign key info will be queried afterwards
                QString(),
                dbschema::ForeignKeyAction::Unknown,
                dbschema::ForeignKeyAction::Unknown,
            });
        }

        // Query foreign key relations for the table
        QSqlQuery fkq{m_db};
        fkq.setForwardOnly(true);
        if(!fkq.exec(u"PRAGMA %1.foreign_key_list(\"%2\")"_s.arg(schema.name, table.name))) {
            qWarning() << "Failed to get foreign key info for" << table.name << q.lastError().text();
            return;
        }

        while(fkq.next()) {
            const QString fromColumn = fkq.value(3).toString();
            for(dbschema::Column &col : table.columns) {
                if(col.name == fromColumn) {
                    col.foreignKeyToTable = fkq.value(2).toString();
                    col.foreignKeyToColumn = fkq.value(3).toString();
                    col.onUpdate = dbschema::foreignKeyActionFromString(fkq.value(4).toString());
                    col.onDelete = dbschema::foreignKeyActionFromString(fkq.value(5).toString());
                    break;
                }
            }
        }
    }

    m_schema = {
        schemas,
        schemas.length() > 1
    };

    Q_EMIT schemaRefreshed(m_schema);
}
