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

#include "../connection.h"

namespace db {

class Sqlite3Connection : public Connection
{
    Q_OBJECT
public:
	Sqlite3Connection(const QUrl& url, QObject *parent=nullptr);

	QString name() const;
	bool isCapable(Capability capability) const;

protected:
	QString type() const { return QString("QSQLITE"); }
	void prepareConnection(QSqlDatabase &db);

protected slots:
	void doGetDbStructure();
	void doGetDbList();
	void doGetCreateScript(const QString& table) override;
};

}

#endif // SQLITE3CONNECTION_H
