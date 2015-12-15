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

namespace db {

ServerConnection::ServerConnection(const QUrl& url, QObject *parent) :
	Connection(url, parent)
{
}

QString ServerConnection::name() const
{
	QString p = url().path();
	if(p.startsWith("/"))
		p = p.mid(1);
	return p;
}

void ServerConnection::prepareConnection(QSqlDatabase &db)
{
	const QUrl& u = url();
	db.setHostName(u.host());
	db.setPort(u.port(defaultPort()));
	db.setUserName(u.userName());
	db.setPassword(u.password());
	db.setDatabaseName(name());
}

}

