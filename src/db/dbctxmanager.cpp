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
