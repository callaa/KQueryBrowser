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
#ifndef CONNECTION_H
#define CONNECTION_H

#include <QThread>
#include <QSqlDatabase>
#include <QMutex>

#include <KUrl>

#include "../meta/schema.h"

class QSqlError;
class QStringList;

class DbCtxManager;
class Database;

/**
  \brief A database connection thread.
  */
class Connection : public QThread
{
	friend class DbCtxManager;
    Q_OBJECT
public:
    explicit Connection(const KUrl &url, QObject *parent = 0);
	~Connection();

	/**
	 * \brief special features supported by the connection handler
	 */
	enum Capability {
		//! Can switch the current database
		SWITCH_DB,

		//! Can show table creation script
		SHOW_CREATE
	};

	/**
	 \brief Create a new database connection.
	
	 The connection type is decided based on the URL scheme component.
	 The connection is not automatically opened. Connect the signals and
	 call start() to open it. The signal opened() will be emitted when
	 the connection is successfully opened. In case of error,
	 cannotOpen(QString) is emitted. 

	 \param url the database URL
	 \param parent the parent object for the connection
	 \return new connection or 0 if type is unrecognized
	 */
	static Connection *create(const KUrl& url, QObject *parent = 0);

	/**
	  \brief Connect the signals and slots for a query browser window

	  This function emits a signal that is sent to the database context manager,
	  which will create a new context for the query tool object.
	  The following signals and slots will be connected:
	  */
	void connectContext(QObject *querytool);

	/**
	  \brief Request emission of latest dbStructure
	  
	  This function just emits the needDbStructure signal from the
	  context of this object.
	  */
	void getDbStructure();

	/**
	  \brief Request emission of latest dbList

	  This function just emits the needDbList signal from the
	  context of this object.
	  */
	void getDbList();

	/**
	  \brief Get the name of the connection

	  The name depends on the connection type, but is usually
	  the name of the database.
	  */
	virtual QString name() const = 0;

	/**
	 \brief Get the URL that was used to open this connection
	 */
	KUrl url() const;

	/**
	 * \brief Check if this connection handler has the given feature
	 * \return true if feature is available
	 */
	virtual bool isCapable(Capability capability) const = 0;

signals:
	//! The database connection was opened succesfully
	void opened();

	//! Couldn't open connection
	void cannotOpen(const QString& message);

	/**
	 * \brief The name of this connection has changed.
	 *
	 * This is emitted when the connection URL is changed.
	 * \param name the new name
	 */
	void nameChanged(const QString& name);

	/**
	 \brief Request new context for an object from the context manager

	 This is a connected to the context manager via a blocking queued connection.
	 */

	void needNewContext(QObject *forthis);

	//! Request new dbStructure
	void needDbStructure();

	//! Request new dbList
	void needDbList();

	/**
	 * \brief Request generation of a table creation script.
	 * \param table the table name
	 */
	void needCreateTable(const QString& table);

	/**
	 * \brief Request switching of connected database
	 * \param database name of the new database
	 */
	void switchDatabase(const QString& database);

	//! Updated database structure info
	void dbStructure(const Database& tables);

	/**
	 * \brief Updated database list
	 * \param databases list of available database names
	 * \param current the currently connected database
	 */
	void dbList(const QStringList& databases, const QString& current);

	/**
	 * \brief A new script has been generated
	 *
	 * The script can be shown in a new script editor tab.
	 * \param script the newly generated script
	 */
	void newScript(const QString& script);

protected:
	void run();

	/**
	 * \brief Change the connection URL.
	 *
	 * The signal namedChanged(QString) is emitted automatically.
	 * \param newurl the new URL
	 */
	void changeUrl(const KUrl &newurl);

	/**
	  \brief Set connection properties

	  This is called from the run() method just before opening the connection.
	  */
	virtual void prepareConnection(QSqlDatabase &db) = 0;

	/**
	  \brief Get schemas available in the current database

	  The schema list contains all the schemas and their tables available
	  to the current user.
	  If a database does not support schemas, the return value should contain
	  a single schema with an empty name which in turn should contain
	  the list of database tables.

	  This should be called only from this thread.
	  */
	virtual QVector<Schema> schemas() = 0;

	/**
	  \brief Get a list of databases the user can access

	  This should be called only from this thread.
	  */
	virtual QStringList databases() = 0;

	/**
	 * \brief Generate a creation script for a table
	 *
	 * The default implementation returns an empty string
	 * and prints a warning. Reimplement this if your subclass
	 * has the SHOW_CREATE capability.
	 *
	 * \param table the table name
	 */
	virtual QString createScript(const QString& table);

	/**
	 * \brief Switch the currently active database
	 *
	 * The default implementation returns false and prints
	 * a warning. Reimplement this if your subclass has the
	 * SWITCH_DB capability.
	 *
	 * This should be called only from this thread.
	 * \param database the new database
	 * \return true if switch succeeded
	 */
	virtual bool selectDatabase(const QString& database);

	/**
	  \brief Return the Qt SQL driver type for this connection
	  */
	virtual QString type() const = 0;

	//! Database connection
	QSqlDatabase m_db;

private:
	//! Number of connections opened (this is used to make unique connection names)
	static int m_count;

	KUrl m_url;
	QMutex m_urlmutex;
};

#endif // CONNECTION_H
