#ifndef DBCONTEXT_H
#define DBCONTEXT_H

#include <QObject>

class QSqlDatabase;
class QueryResults;
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

	  A results signal will be emitted once the query is complete.
	  */
	void doQuery(const QString& querystr, int limit);

private:
	QSqlDatabase &m_db;

	// This points to an object in another thread. It is used only to identify this context
	QObject *m_target;

};

#endif // DBCONTEXT_H
