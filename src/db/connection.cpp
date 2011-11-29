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

#include "connection.h"
#include "dbctxmanager.h"

int Connection::m_count = 0;

Connection::Connection(QObject *parent) :
	QThread(parent)
{
}

Connection::~Connection()
{
	quit();
	wait();
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
		connect(this, SIGNAL(needNewContext(QObject*)), ctxman, SLOT(createContext(QObject*)), Qt::BlockingQueuedConnection);
		connect(this, SIGNAL(needDbStructure()), ctxman, SLOT(getDbStructure()), Qt::QueuedConnection);
		connect(this, SIGNAL(needDbList()), ctxman, SLOT(getDbList()), Qt::QueuedConnection);
		connect(ctxman, SIGNAL(dbStructure(Database)), this, SIGNAL(dbStructure(Database)), Qt::QueuedConnection);
		connect(ctxman, SIGNAL(dbList(QStringList)), this, SIGNAL(dbList(QStringList)), Qt::QueuedConnection);
		emit opened();

		exec();
		delete ctxman;

	} else {
		QString error = m_db.lastError().text();
		m_db = QSqlDatabase();
		QSqlDatabase::removeDatabase(dbname);
		qDebug() << "Connection error:" << error;
		emit cannotOpen(error);
	}
}
