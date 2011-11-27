#ifndef SQLITE3CONNECTION_H
#define SQLITE3CONNECTION_H

#include "connection.h"

class Sqlite3Connection : public Connection
{
    Q_OBJECT
public:
	explicit Sqlite3Connection(const QString& dbpath, QObject *parent = 0);

protected:
	QString type() const { return QString("QSQLITE"); }

	void prepareConnection(QSqlDatabase &db);

	QVector<Schema> schemas();

private:
	QString m_dbpath;
};

#endif // SQLITE3CONNECTION_H
