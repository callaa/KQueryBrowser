//
// This file is part of KQueryBrowser.
//
// KQueryBrowser is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KQueryBrowser is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KQueryBrowser.  If not, see <http://www.gnu.org/licenses/>.
//
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

	QString name() const { return m_database; }
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
