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
#ifndef DBCONTEXT_H
#define DBCONTEXT_H

#include <QObject>

class QSqlDatabase;
class QSqlQuery;
class QueryResults;
class QueryResultsData;
class Tables;

/**
  \brief A database context

  A database context holds state on the database thread relevant to a query/script window.
  */
class DbContext : public QObject
{
    Q_OBJECT
public:
	explicit DbContext(QObject *target, QSqlDatabase &db, QObject *parent = 0);
	~DbContext();

	bool isForTarget(const QObject *target) const { return m_target == target; }

signals:
	void results(const QueryResults& results);

public slots:
	/**
	  \brief Perform a database query.

	  A results signal will be emitted once the query is complete or limit reached.
	  */
	void doQuery(const QString& querystr, int limit);

	/**
	  \brief Request more results from the currently open query.

	  A results signal will be emitted once the query is complete or limit reached.
	  */
	void getMoreResults(int limit);

private:
	/**
	  \brief Get new results and emit results().
	  */
	void getNewResults(int limit);

	/**
	  Get rows from the current query and put them in the result data
	  \param data the result data set
	  \param cols number of columns in the query result set
	  \param limit maximum number of results to get (<=0 for unlimited)
	  */
	void gatherRows(QueryResultsData *data, int cols, int limit);

	QSqlDatabase &m_db;

	QSqlQuery *m_query;

	// This points to an object in another thread. It is used only to identify this context
	QObject *m_target;

};

#endif // DBCONTEXT_H
