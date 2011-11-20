#include <QDebug>
#include <QSqlRecord>
#include <QStringList>

#include "dbctxmanager.h"
#include "dbcontext.h"
#include "connection.h"
#include "database.h"

DbCtxManager::DbCtxManager(Connection *connection) :
	QObject(), m_connection(connection)
{
}

void DbCtxManager::createContext(const QObject *forthis)
{
	qDebug() << "Creating a new context!";
	DbContext *ctx = new DbContext(m_connection->m_db, this);
	connect(forthis, SIGNAL(doQuery(QString, int)), ctx, SLOT(doQuery(QString, int)));
	connect(ctx, SIGNAL(results(QueryResults)), forthis, SLOT(queryResults(QueryResults)));
}

static void addTables(QVector<Table> &tables, QSqlDatabase &db, const QStringList& names, bool isview) {
	foreach(const QString& t, names) {
		QSqlRecord r = db.record(t);
		QVector<Column> cols(r.count());
		for(int i=0;i<r.count();++i)
			cols[i] = Column(r.fieldName(i));
		tables.append(Table(t, cols, isview));
	}
}

void DbCtxManager::getDbStructure()
{
	QVector<Table> tables;
	addTables(tables, m_connection->m_db, m_connection->m_db.tables(QSql::Tables), false);
	addTables(tables, m_connection->m_db, m_connection->m_db.tables(QSql::Views), true);

	emit dbStructure(Database(tables));
}
