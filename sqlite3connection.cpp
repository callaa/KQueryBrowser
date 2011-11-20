#include <QSqlDatabase>

#include "sqlite3connection.h"

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
	// TODO
}
