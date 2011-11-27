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
#include <QDebug>
#include <QSqlRecord>
#include <QStringList>

#include "dbctxmanager.h"
#include "dbcontext.h"
#include "connection.h"
#include "../database.h"

DbCtxManager::DbCtxManager(Connection *connection) :
	QObject(), m_connection(connection)
{
}

void DbCtxManager::createContext(QObject *forthis)
{
	qDebug() << "Creating a new context!";
	DbContext *ctx = new DbContext(forthis, m_connection->m_db, this);
	connect(forthis, SIGNAL(doQuery(QString, int)), ctx, SLOT(doQuery(QString, int)));
	connect(forthis, SIGNAL(getMoreResults(int)), ctx, SLOT(getMoreResults(int)));
	connect(ctx, SIGNAL(results(QueryResults)), forthis, SLOT(queryResults(QueryResults)));
	connect(forthis, SIGNAL(destroyed(QObject*)), this, SLOT(removeContext(QObject*)));
}

void DbCtxManager::removeContext(QObject *forthis)
{
	DbContext *delctx = 0;
	foreach(QObject *c, this->children()) {
		DbContext *ctx = qobject_cast<DbContext*>(c);
		if(ctx!=0 && ctx->isForTarget(forthis)) {
			delctx = ctx;
			break;
		}
	}
	delete delctx;
}

void DbCtxManager::getDbStructure()
{
	emit dbStructure(Database(m_connection->schemas()));
}
