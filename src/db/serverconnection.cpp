#include "serverconnection.h"

ServerConnection::ServerConnection(QObject *parent) :
	Connection(parent), m_port(-1)
{
}

void ServerConnection::prepareConnection(QSqlDatabase &db)
{
	db.setHostName(m_host);
	db.setPort(m_port>0 ? m_port : defaultPort());
	db.setUserName(m_username);
	db.setPassword(m_password);
	db.setDatabaseName(m_database);
}
