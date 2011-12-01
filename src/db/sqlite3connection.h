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
#ifndef SQLITE3CONNECTION_H
#define SQLITE3CONNECTION_H

#include "connection.h"

class Sqlite3Connection : public Connection
{
    Q_OBJECT
public:
	Sqlite3Connection(const KUrl& url, QObject *parent);

	QString name() const;
protected:
	QString type() const { return QString("QSQLITE"); }

	void prepareConnection(QSqlDatabase &db);

	QVector<Schema> schemas();
	QStringList databases();
};

#endif // SQLITE3CONNECTION_H
