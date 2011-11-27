#ifndef CONNECTION_H
#define CONNECTION_H

#include <QThread>
#include <QSqlDatabase>

#include "../schema.h"

class QSqlError;

class DbCtxManager;
class Database;
class Table;

/**
  \brief A database connection thread.
  */
class Connection : public QThread
{
	friend class DbCtxManager;
    Q_OBJECT
public:
    explicit Connection(QObject *parent = 0);
	~Connection();

	/**
	  \brief Connect the signals and slots for a query browser window

	  This function emits a signal that is sent to the database context manager,
	  which will create a new context for the query tool object.
	  The following signals and slots will be connected:
	  */
	void connectContext(QObject *querytool);

	/**
	  \brief Request emission of latest dbStructure
	  */
	void getDbStructure();

signals:
	//! The database connection was opened succesfully
	void opened();

	//! Couldn't open connection
	void cannotOpen(const QString& message);

	//! Internal use only: Request new context for an object from the context manager
	void needNewContext(QObject *forthis);

	//! Internal use only: Request new dbStructure
	void needDbStructure();

	//! Updated database structure info
	void dbStructure(const Database& tables);

protected:
	void run();

	/**
	  \brief Set connection properties
	  */
	virtual void prepareConnection(QSqlDatabase &db) = 0;

	/**
	  \brief Get schemas available in the current database
	  */
	virtual QVector<Schema> schemas() = 0;

	/**
	  \brief Return the Qt SQL driver type for this connection
	  */
	virtual QString type() const = 0;

	//! Database connection
	QSqlDatabase m_db;

private:
	//! Number of connections opened (this is used to make unique connection names)
	static int m_count;
};

#endif // CONNECTION_H
