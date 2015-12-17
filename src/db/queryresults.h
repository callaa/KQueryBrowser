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
#ifndef QUERYRESULTS_H
#define QUERYRESULTS_H

#include <QMetaType>
#include <QVariant>
#include <QVector>
#include <QSharedDataPointer>

#include "../meta/column.h"

namespace db {

class Query;

typedef QVector<QVariant> ResultRow;
typedef QVector<ResultRow> ResultRows;

struct QueryResultsData : public QSharedData
{
	QueryResultsData();
	bool select, success, more, continuation;
	int rowcount;
	QString error;
	ResultRows rows;
	QVector<meta::Column> columns;
};

class QueryResults
{
	friend class Query;
public:
	QueryResults();

	/**
	  \brief Was the query a SELECT type query
	*/
	bool isSelect() const { return m_d->select; }

	/**
	  \brief Get the number of affected rows

	  This is the number of rows affected by a non-SELECT query,
	  or the total number of rows returned by SELECT (if available)

	  \return number of rows affected/returned by query or -1 if unknown
	  */
	int rowCount() const { return m_d->rowcount; }

	/**
	  \brief Was the query successful?
	  */
	bool success() const { return m_d->success; }

	/**
	  \brief Get the error message.
	  If success() returns false, use this to get the error message.
	  */
	const QString& error() const  { return m_d->error; }

	/**
	  \brief Is there more data available?
	  \return true if this is not the whole result set
	  */
	bool isMore() const { return m_d->more; }

	/**
	  \brief Is this result set more rows for the last query?
	  \return true if this is a continuation of the previous query
	  */
	bool isContinuation() const { return m_d->continuation; }

	/**
	  \brief Get the rows returned by the (SELECT) query.
	  \return rows
	  */
	const ResultRows &rows() const { return m_d->rows; }

	/**
	  \brief Get the column metadata
	  \return list of columns
	  */
	const QVector<meta::Column> columns() const { return m_d->columns; }

private:
	QueryResults(QueryResultsData *data);

	QSharedDataPointer<QueryResultsData> m_d;
};

}

Q_DECLARE_METATYPE(db::QueryResults)

#endif // QUERYRESULTS_H
