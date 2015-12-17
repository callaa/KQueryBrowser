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

namespace db {

/**
 * \brief Base class for server based database connections.
 *
 * This class provides a prepareConnection implementation that sets
 * the database host, port, username, password and database name.
 */
class ServerConnection : public Connection
{
    Q_OBJECT
public:
	ServerConnection(const QUrl& url, QObject *parent);

	QString name() const override;

protected:
	void prepareConnection(QSqlDatabase &db) override;

	/**
	 * \brief Get the default port
	 *
	 * Concrete classes must reimplement this method to return
	 * the default port number for the database server.
	 * This number is used if no port has been explicitly set in the URL.
	 */
	virtual int defaultPort() const = 0;
};

}

#endif // SERVERCONNECTION_H
