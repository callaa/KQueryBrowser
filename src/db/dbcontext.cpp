#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <climits>

#include "dbcontext.h"
#include "../queryresults.h"

DbContext::DbContext(QObject *target, QSqlDatabase &db, QObject *parent) :
	QObject(parent), m_db(db), m_query(0), m_target(target)
{
}

DbContext::~DbContext()
{
	delete m_query;
}

void DbContext::doQuery(const QString &querystr, int limit)
{
	delete m_query;
	m_query = new QSqlQuery(m_db);
	m_query->setForwardOnly(true);

	if(m_query->exec(querystr)) {
		// No errors, get results
		getNewResults(limit);
	} else {
		// An error occurred. Report error
		QueryResultsData *res = new QueryResultsData();
		res->select = m_query->isSelect();
		QString error;
		switch(m_query->lastError().type()) {
			case QSqlError::ConnectionError: error = tr("Connection error"); break;
			case QSqlError::StatementError: error = tr("Syntax error"); break;
			case QSqlError::TransactionError: error = tr("Transaction error"); break;
			default: error = tr("Unknown error"); break;
		}

		res->error = QString("%1 [%2]: %3").arg(error).arg(m_query->lastError().number()).arg(m_query->lastError().databaseText());

		emit results(QueryResults(res));
		delete m_query;
		m_query = 0;
	}

}

void DbContext::getNewResults(int limit)
{
	QueryResultsData *res = new QueryResultsData();
	res->success = true;
	if(m_query->isSelect()) {
		res->select = true;
		res->rowcount = m_query->size();

		// Get column headers
		const int cols = m_query->record().count();
		res->columns.resize(cols);
		for(int i=0;i<cols;++i)
			res->columns[i] = Column(m_query->record().fieldName(i));

		// Get row data
		gatherRows(res, cols, limit);
	} else {
		res->rowcount = m_query->numRowsAffected();
	}

	emit results(QueryResults(res));

	// If limit has not been reached, see if there are more results
	if(!res->more) {
		if(m_query->nextResult()) {
			getNewResults(limit);
		} else {
			delete m_query;
			m_query = 0;
		}
	}
}

void DbContext::getMoreResults(int limit)
{
	QueryResultsData *res = new QueryResultsData();
	res->continuation = true;
	res->select = true;
	if(m_query==0) {
		res->error = tr("No more results!");
	} else {
		res->success = true;
		gatherRows(res, m_query->record().count(), limit);
	}

	emit results(QueryResults(res));

	if(!res->more) {
		if(m_query->nextResult()) {
			getNewResults(limit);
		} else {
			delete m_query;
			m_query = 0;
		}
	}
}

void DbContext::gatherRows(QueryResultsData *res, int cols, int limit)
{
	// Note. This is technically different than no limit at all, but
	// practically, nobody is going to read INT_MAX rows anyway.
	if(limit<=0)
		limit = INT_MAX;

	int count=0;
	while(m_query->next() && ++count<limit) {
		ResultRow row(cols);
		for(int i=0;i<cols;++i)
			row[i] = m_query->value(i);
		res->rows.append(row);
	}
	res->more = m_query->isValid();
}
