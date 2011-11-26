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
	void gatherRows(QueryResultsData *data, int cols, int limit);

	QSqlDatabase &m_db;

	QSqlQuery *m_query;

	// This points to an object in another thread. It is used only to identify this context
	QObject *m_target;

};

#endif // DBCONTEXT_H
