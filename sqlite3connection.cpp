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

void Sqlite3Connection::setExtraInfo(Table &table)
{
	QSqlQuery q("pragma table_info(" + table.name() + ")", m_db);

	// Pragma columns are: column id (zero based), column name, column type, not-null, default value, primary key
	while(q.next()) {
		Column &c = table.columns()[q.value(0).toInt()];
		c.setExtraInfo(
					q.value(5).toBool(), // PK
					q.value(2).toString()); // type
	}
}
