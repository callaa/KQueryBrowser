#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QVariant>

#include "sqlite3connection.h"
#include "table.h"

Sqlite3Connection::Sqlite3Connection(const QString& dbpath, QObject *parent) :
	Connection(parent), m_dbpath(dbpath)
{
}

void Sqlite3Connection::prepareConnection(QSqlDatabase &db)
{
	db.setDatabaseName(m_dbpath);
}

QVector<Schema> Sqlite3Connection::schemas()
{
	QVector<Table> tables;

	QSqlQuery q("SELECT type, name FROM sqlite_master WHERE type IN ('table', 'view') ORDER BY type, name ASC", m_db);
	while(q.next()) {
		tables.append(Table(q.value(1).toString(), QVector<Column>(), q.value(0).toString() == "table" ? Table::TABLE : Table::VIEW));
	}
	// Add master table to the list too
	tables.append(Table("sqlite_master", QVector<Column>(), Table::SYSTEMTABLE));

	// Get column definitions
	for(int i=0;i<tables.count();++i) {
		Table &t = tables[i];

		q.exec("pragma table_info(" + t.name() + ")");

		// Pragma columns are: column id (zero based), column name, column type, not-null, default value, primary key
		while(q.next()) {
			Column c = Column(q.value(1).toString());
			c.setType(q.value(2).toString());
			c.setPk(q.value(5).toBool());
			t.columns().append(c);
		}
	}

	QVector<Schema> schemas(1);
	schemas[0] = Schema(QString(), tables);
	return schemas;
}

