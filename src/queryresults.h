#ifndef QUERYRESULTS_H
#define QUERYRESULTS_H

#include <QMetaType>
#include <QVariant>
#include <QVector>
#include <QSharedDataPointer>

#include "column.h"

class DbContext;

typedef QVector<QVariant> ResultRow;
typedef QVector<ResultRow> ResultRows;

struct QueryResultsData : public QSharedData
{
	QueryResultsData();
	bool select, success, more, continuation;
	int rowcount;
	QString error;
	ResultRows rows;
	QVector<Column> columns;
};

class QueryResults
{
	friend class DbContext;
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
	const QVector<Column> columns() const { return m_d->columns; }

private:
	QueryResults(QueryResultsData *data);

	QSharedDataPointer<QueryResultsData> m_d;
};

Q_DECLARE_METATYPE(QueryResults)

#endif // QUERYRESULTS_H
