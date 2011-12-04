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
#ifndef DBCTXMANAGER_H
#define DBCTXMANAGER_H

#include <QObject>

class QStringList;

class Connection;
class Database;

class DbCtxManager : public QObject
{
    Q_OBJECT
public:
	explicit DbCtxManager(Connection *connection);

signals:
	void dbStructure(const Database& db);
	void dbList(const QStringList& databases, const QString& current);
	void newScript(const QString& script);

public slots:
	void createContext(QObject *forthis);
	void removeContext(QObject *forthis);
	void getDbStructure();
	void getDbList();
	void makeCreateTable(const QString& table);
	void switchDatabase(const QString& database);

private:
	Connection *m_connection;

};

#endif // DBCTXMANAGER_H
