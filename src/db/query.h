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
#ifndef DB_QUERY_H
#define DB_QUERY_H

#include <QObject>
#include <QScopedPointer>

class QSqlDatabase;
class QSqlQuery;

namespace db {

class Connection;
class QueryResults;
class QueryResultsData;

/**
  \brief A database query context.
  */
class Query : public QObject
{
    Q_OBJECT
public:
	explicit Query(QSqlDatabase &db, QObject *parent=nullptr);
	~Query();

	/**
	  \brief Perform a database query.

	  The signal "results" will be emitted once complete.
	  */
	void query(const QString& querystr, int limit);

	/**
	  \brief Request more results from the currently open query.

	  A results signal will be emitted once the query is complete or limit reached.
	  */
	void getMoreResults(int limit);
	
signals:
	// note: namespace specified so it can be used with string based signals and slots
	void results(const db::QueryResults& results);

private slots:
	void doQuery(const QString& querystr, int limit);
	void doGetMoreResults(int limit);

private:
	void getNewResults();
	void gatherRows(QueryResultsData *data, int cols);


	QSqlDatabase &m_db;
	QScopedPointer<QSqlQuery> m_query;
	int m_limit;
};

}

Q_DECLARE_METATYPE(db::Query*)

#endif

