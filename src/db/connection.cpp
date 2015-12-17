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
#include <QThread>

#include "connection.h"
#include "query.h"

#include "backend/sqlite3connection.h"
#include "backend/mysqlconnection.h"
#include "backend/pgsqlconnection.h"

namespace db {

static QThread *DB_THREAD;

int Connection::m_count = 0;

Connection::Connection(const QUrl& url, QObject *parent) :
	QObject(parent), m_url(url)
{
}

Connection::~Connection()
{
	QString name = m_db.connectionName();
	m_db.close();
	QSqlDatabase::removeDatabase(name);
}

Connection *Connection::create(const QUrl& url)
{
	Connection *c = nullptr;
	if(url.scheme() == "sqlite3")
		c = new Sqlite3Connection(url);
	else if(url.scheme() == "mysql")
		c = new MysqlConnection(url);
	else if(url.scheme() == "pgsql")
		c = new PgsqlConnection(url);
	else
		qWarning() << "Unhandled scheme:" << url.scheme();

	if(c) {
		if(!DB_THREAD) {
			DB_THREAD = new QThread;
			DB_THREAD->start();
		}

		c->moveToThread(DB_THREAD);
	}
	return c;
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


void Connection::open()
{
	QMetaObject::invokeMethod(this, "doOpen");
}

void Connection::createQuery(QObject *notifyObject, const QByteArray &notifyMethod)
{
	QMetaObject::invokeMethod(this, "doCreateQuery", Qt::AutoConnection, Q_ARG(QObject*, notifyObject), Q_ARG(const QByteArray&, notifyMethod));
}

void Connection::doCreateQuery(QObject *notifyObject, const QByteArray &notifyMethod)
{
	Query *q = new Query(m_db, this);
	QMetaObject::invokeMethod(notifyObject, notifyMethod.constData(), Qt::AutoConnection, Q_ARG(db::Query*, q));
}

void Connection::doOpen()
{
	qDebug() << "Opening database connection" << m_count;
	QString dbname = QString("c%1").arg(m_count++);
	m_db = QSqlDatabase::addDatabase(type(), dbname);

	prepareConnection(m_db);

	if(m_db.open()) {
		emit opened();

	} else {
		QString error = m_db.lastError().text();
		qDebug() << "Connection error:" << error;
		emit cannotOpen(error);
	}
}

void Connection::getDbStructure()
{
	QMetaObject::invokeMethod(this, "doGetDbStructure");
}

void Connection::getDbList()
{
	QMetaObject::invokeMethod(this, "doGetDbList");
}

void Connection::getCreateScript(const QString &table)
{
	if(isCapable(SHOW_CREATE))
		QMetaObject::invokeMethod(this, "doGetCreateScript", Q_ARG(QString, table));
	else
		qWarning() << "This connection type does not support create script generation!";
}

void Connection::doGetCreateScript(const QString& table)
{
	Q_UNUSED(table);
	qWarning("createScript() not implemented for this connection type!");
}

void Connection::switchDatabase(const QString &database)
{
	if(isCapable(SWITCH_DB))
		QMetaObject::invokeMethod(this, "doSwitchDatabase", Q_ARG(QString, database));
	else
		qWarning() << "This connection type does not support switching active databases!";
}

void Connection::doSwitchDatabase(const QString& database)
{
	Q_UNUSED(database);
	qWarning("doSwitchDatabase() not implemented for this connection type!");
}

}

