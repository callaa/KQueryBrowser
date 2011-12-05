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

#include "sqlite3connection.h"
#include "mysqlconnection.h"
#include "pgsqlconnection.h"

int Connection::m_count = 0;

Connection::Connection(const KUrl& url, QObject *parent) :
	QThread(parent), m_url(url)
{
}

Connection::~Connection()
{
	quit();
	wait();
}

Connection *Connection::create(const KUrl& url, QObject *parent)
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

KUrl Connection::url() const
{
	QMutexLocker l(const_cast<QMutex*>(&m_urlmutex));
	return m_url;
}

void Connection::changeUrl(const KUrl& url)
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
		connect(this, SIGNAL(needNewContext(QObject*)),
				ctxman, SLOT(createContext(QObject*)), Qt::BlockingQueuedConnection);
		connect(this, SIGNAL(needDbStructure()),
				ctxman, SLOT(getDbStructure()), Qt::QueuedConnection);
		connect(this, SIGNAL(needDbList()),
				ctxman, SLOT(getDbList()), Qt::QueuedConnection);
		connect(this, SIGNAL(needCreateTable(QString)),
				ctxman, SLOT(makeCreateTable(QString)), Qt::QueuedConnection);
		connect(this, SIGNAL(switchDatabase(QString)),
				ctxman, SLOT(switchDatabase(QString)), Qt::QueuedConnection);
		connect(ctxman, SIGNAL(dbStructure(Database)),
				this, SIGNAL(dbStructure(Database)), Qt::QueuedConnection);
		connect(ctxman, SIGNAL(dbList(QStringList,QString)),
				this, SIGNAL(dbList(QStringList, QString)), Qt::QueuedConnection);
		connect(ctxman, SIGNAL(newScript(QString)),
				this, SIGNAL(newScript(QString)), Qt::QueuedConnection);
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

