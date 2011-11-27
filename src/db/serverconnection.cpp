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
