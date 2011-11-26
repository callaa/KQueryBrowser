#ifndef SERVERCONNECTION_H
#define SERVERCONNECTION_H

#include "connection.h"

class ServerConnection : public Connection
{
    Q_OBJECT
public:
	explicit ServerConnection(QObject *parent = 0);

	void setServer(const QString& host) { m_host = host; }
	void setPort(int port) { m_port = port; }
	void setUsername(const QString& username) { m_username = username; }
	void setPassword(const QString& password) { m_password = password; }
	void setDatabase(const QString& database) { m_database = database; }

protected:
	void prepareConnection(QSqlDatabase &db);
	virtual int defaultPort() const = 0;

private:
	QString m_host;
	int m_port;
	QString m_username, m_password;
	QString m_database;
};

#endif // SERVERCONNECTION_H
