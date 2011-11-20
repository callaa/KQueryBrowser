#ifndef CONNECTION_H
#define CONNECTION_H

#include <QThread>
#include <QSqlDatabase>

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
	void connectContext(const QObject *querytool);

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
	void needNewContext(const QObject *forthis);

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
	  \brief Set table metadata that is not available via QSqlRecord
	  */
	virtual void setExtraInfo(Table& table) = 0;

	/**
	  \brief Return the Qt SQL driver type for this connection
	  */
	virtual QString type() const = 0;

private:
	//! Database connection
	QSqlDatabase m_db;

	//! Number of connections opened (this is used to make unique connection names)
	static int m_count;
};

#endif // CONNECTION_H
