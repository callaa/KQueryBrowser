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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlField>
#include <QSqlError>
#include <QTimer>

#include <climits>

#include "query.h"
#include "queryresults.h"

namespace db {

Query::Query(QSqlDatabase &db, QObject *parent) :
	QObject(parent), m_db(db)
{
}

Query::~Query()
{
}

void Query::query(const QString &querystr, int limit)
{
	QMetaObject::invokeMethod(this, "doQuery", Qt::AutoConnection, Q_ARG(QString, querystr), Q_ARG(int, limit));
}

void Query::doQuery(const QString &querystr, int limit)
{
	m_limit = limit;
	m_query.reset(new QSqlQuery(m_db));
	m_query->setForwardOnly(true);

	if(m_query->exec(querystr)) {
		// No errors, get results
		getNewResults();

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
		m_query.reset();
	}

}

void Query::getNewResults()
{
	if(m_query.isNull()) {
		qWarning() << "getNewResults: no active query!";
		return;
	}

	QueryResultsData *res = new QueryResultsData();
	res->success = true;
	if(m_query->isSelect()) {
		res->select = true;
		res->rowcount = m_query->size();

		// Get column headers
		const int cols = m_query->record().count();
		res->columns.resize(cols);
		for(int i=0;i<cols;++i) {
			QSqlField f = m_query->record().field(i);
			res->columns[i] = meta::Column(f.name());
			res->columns[i].setType(QVariant::typeToName(f.type()));
		}

		// Get row data
		gatherRows(res, cols);
	} else {
		res->rowcount = m_query->numRowsAffected();
	}

	emit results(QueryResults(res));

	// If limit has not been reached, see if there are more results
	if(!res->more) {
		if(m_query->nextResult()) {
			QTimer::singleShot(1, this, &Query::getNewResults);
		} else {
			m_query.reset();
		}
	}
}

void Query::getMoreResults(int limit)
{
	QMetaObject::invokeMethod(this, "doGetMoreResults", Qt::AutoConnection, Q_ARG(int, limit));
}

void Query::doGetMoreResults(int limit)
{
	m_limit = limit;
	QueryResultsData *res = new QueryResultsData();
	res->continuation = true;
	res->select = true;

	if(m_query.isNull()) {
		res->error = tr("No more results!");
	} else {
		res->success = true;
		gatherRows(res, m_query->record().count());
	}

	emit results(QueryResults(res));

	if(!res->more) {
		if(m_query->nextResult()) {
			QTimer::singleShot(1, this, &Query::getNewResults);
		} else {
			m_query.reset();
		}
	}
}

void Query::gatherRows(QueryResultsData *res, int cols)
{
	// Note. This is technically different than no limit at all, but
	// practically, nobody is going to read INT_MAX rows anyway.
	if(m_limit<=0)
		m_limit = INT_MAX;

	int count=0;
	while(m_query->next() && ++count<m_limit) {
		ResultRow row(cols);
		for(int i=0;i<cols;++i)
			row[i] = m_query->value(i);
		res->rows.append(row);
	}
	res->more = m_query->isValid();
}

}

