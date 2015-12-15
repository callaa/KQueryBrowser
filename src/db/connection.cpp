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
#include <QSqlError>
#include <QMutexLocker>

#include "connection.h"
#include "dbctxmanager.h"

#include "backend/sqlite3connection.h"
#include "backend/mysqlconnection.h"
#include "backend/pgsqlconnection.h"

namespace db {

int Connection::m_count = 0;

Connection::Connection(const QUrl& url, QObject *parent) :
	QThread(parent), m_url(url)
{
}

Connection::~Connection()
{
	quit();
	wait();
}

Connection *Connection::create(const QUrl& url, QObject *parent)
{
	if(url.scheme() == "sqlite3")
		return new Sqlite3Connection(url, parent);
	else if(url.scheme() == "mysql")
		return new MysqlConnection(url, parent);
	else if(url.scheme() == "pgsql")
		return new PgsqlConnection(url, parent);
	else
		return 0;
}

QUrl Connection::url() const
{
	QMutexLocker l(const_cast<QMutex*>(&m_urlmutex));
	return m_url;
}

void Connection::changeUrl(const QUrl& url)
{
	{
		QMutexLocker l(&m_urlmutex);
		m_url = url;
	}
	emit nameChanged(name());
}

void Connection::connectContext(QObject *querytool)
{
	emit needNewContext(querytool);
}

void Connection::getDbStructure()
{
	emit needDbStructure();
}

void Connection::getDbList()
{
	emit needDbList();
}

void Connection::run()
{
	qDebug() << "Opening database connection" << m_count;
	QString dbname = QString("c%1").arg(m_count++);
	m_db = QSqlDatabase::addDatabase(type(), dbname);

	prepareConnection(m_db);

	if(m_db.open()) {
		DbCtxManager *ctxman = new DbCtxManager(this);
		connect(this, &Connection::needNewContext,
				ctxman, &DbCtxManager::createContext, Qt::BlockingQueuedConnection);
		connect(this, &Connection::needDbStructure,
				ctxman, &DbCtxManager::getDbStructure, Qt::QueuedConnection);
		connect(this, &Connection::needDbList,
				ctxman, &DbCtxManager::getDbList, Qt::QueuedConnection);
		connect(this, &Connection::needCreateTable,
				ctxman, &DbCtxManager::makeCreateTable, Qt::QueuedConnection);
		connect(this, &Connection::switchDatabase,
				ctxman, &DbCtxManager::switchDatabase, Qt::QueuedConnection);
		connect(ctxman, &DbCtxManager::dbStructure,
				this, &Connection::dbStructure, Qt::QueuedConnection);
		connect(ctxman, &DbCtxManager::dbList,
				this, &Connection::dbList, Qt::QueuedConnection);
		connect(ctxman, &DbCtxManager::newScript,
				this, &Connection::newScript, Qt::QueuedConnection);
		emit opened();

		exec();
		delete ctxman;

	} else {
		QString error = m_db.lastError().text();
		qDebug() << "Connection error:" << error;
		emit cannotOpen(error);
	}
	m_db.close();
	m_db = QSqlDatabase();
	QSqlDatabase::removeDatabase(dbname);
}

QString Connection::createScript(const QString& table)
{
	Q_UNUSED(table);
	qWarning("createScript() not implemented for this connection type!");
	return QString();
}

bool Connection::selectDatabase(const QString& database)
{
	Q_UNUSED(database);
	qWarning("selectDatabase() not implemented for this connection type!");
	return false;
}

}

