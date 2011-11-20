#ifndef DBCONTEXT_H
#define DBCONTEXT_H

#include <QObject>

class QSqlDatabase;
class QueryResults;
class Tables;

class DbContext : public QObject
{
    Q_OBJECT
public:
	explicit DbContext(QSqlDatabase &db, QObject *parent = 0);

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

};

#endif // DBCONTEXT_H
