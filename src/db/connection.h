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

#include <QObject>
#include <QSqlDatabase>
#include <QMutex>
#include <QUrl>

class QSqlError;
class QStringList;

namespace meta {
	class Database;
}

namespace db {

class Query;

/**
  \brief A database connection.

  This typically lives in a separate thread.
  */
class Connection : public QObject
{
    Q_OBJECT
public:
    explicit Connection(const QUrl &url, QObject *parent = 0);
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
	
	 The object will be created in a separate thread.

	 The connection type is decided based on the URL scheme component.
	 The connection is not automatically opened. Connect the signals and
	 call start() to open it. The signal opened() will be emitted when
	 the connection is successfully opened. In case of error,
	 cannotOpen(QString) is emitted. 

	 \param url the database URL
	 \param parent the parent object for the connection
	 \return new connection or 0 if type is unrecognized
	 */
	static Connection *create(const QUrl& url);

	static void stopAll();

	/**
	 \brief Open the database connection.

	 Either opened() or cannotOpen() will be emitted.
	*/
	void open();

	/**
	 \brief Create a new query context
	 The given method will be called when the query context is ready.
	*/
	void createQuery(QObject *notifyObject, const QByteArray &notifyMethod);

	/**
	  \brief Request the latest Database structure.
	  
	  dbStructure() will be emitted when ready.
	  */
	void getDbStructure();

	/**
	  \brief Request the latest list of databases

	  dbList() will be emitted when ready
	  */
	void getDbList();

	/**
	  \brief Request a creation script for the given table

	 The signal newScript() will be emitted when ready
	 */
	void getCreateScript(const QString &tablename);

	/**
	 \brief Switch to another database

	 Not all backends support this
	 */
	void switchDatabase(const QString &dbname);

	/**
	  \brief Get the name of the connection

	  The name depends on the connection type, but is usually
	  the name of the database.
	  */
	virtual QString name() const = 0;

	/**
	 \brief Get the URL that was used to open this connection
	 */
	QUrl url() const;

	/**
	 * \brief Check if this connection handler has the given feature
	 * \return true if feature is available
	 */
	virtual bool isCapable(Capability capability) const = 0;

signals:
	//! The database connection was opened succesfully
	void opened();

	//! Couldn't open connection
	void cannotOpen(const QString &message);

	/**
	 * \brief The name of this connection has changed.
	 *
	 * This is emitted when the connection URL is changed.
	 * \param name the new name
	 */
	void nameChanged(const QString& name);

	//! Updated database structure info
	void dbStructure(const meta::Database& tables);

	//! Updated list of databases
	void dbList(const QStringList &databases, const QString &current);

	//! A new script has been generated
	void newScript(const QString& script);

protected:
	/**
	 * \brief Change the connection URL.
	 *
	 * The signal namedChanged(QString) is emitted automatically.
	 * \param newurl the new URL
	 */
	void changeUrl(const QUrl &newurl);

	/**
	  \brief Set connection properties

	  This is called from the doOpen() method just before opening the connection.
	  */
	virtual void prepareConnection(QSqlDatabase &db) = 0;

	/**
	  \brief Return the Qt SQL driver type for this connection
	  */
	virtual QString type() const = 0;

	//! Database connection
	QSqlDatabase m_db;

protected slots:
	virtual void doGetDbStructure() = 0;
	virtual void doGetDbList() = 0;
	virtual void doSwitchDatabase(const QString &name);
	virtual void doGetCreateScript(const QString& table);

private slots:
	void doOpen();
	void doCreateQuery(QObject*, const QByteArray&);

private:
	//! Number of connections opened (this is used to make unique connection names)
	static int m_count;

	QUrl m_url;
	QMutex m_urlmutex;
};

}

#endif // CONNECTION_H
