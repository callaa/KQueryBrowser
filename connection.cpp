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

void Connection::run()
{
	qDebug() << "Opening database connection" << m_count;
	m_db = QSqlDatabase::addDatabase(type(), QString("c%1").arg(m_count++));

	prepareConnection(m_db);

	if(m_db.open()) {
		DbCtxManager *ctxman = new DbCtxManager(this);
		connect(this, SIGNAL(needNewContext(QObject*)), ctxman, SLOT(createContext(QObject*)), Qt::BlockingQueuedConnection);
		connect(this, SIGNAL(needDbStructure()), ctxman, SLOT(getDbStructure()), Qt::QueuedConnection);
		connect(ctxman, SIGNAL(dbStructure(Database)), this, SIGNAL(dbStructure(Database)));
		emit opened();
	} else {
		emit cannotOpen(m_db.lastError().text());
		return;
	}

	exec();
}
