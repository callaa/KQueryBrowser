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
	  
	  This function just emits the needDbStructure signal from the
	  context of this object.
	  */
	void getDbStructure();

signals:
	//! The database connection was opened succesfully
	void opened();

	//! Couldn't open connection
	void cannotOpen(const QString& message);

	//! Request new context for an object from the context manager
	void needNewContext(QObject *forthis);

	//! Request new dbStructure
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
