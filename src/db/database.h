#ifndef DATABASE_H
#define DATABASE_H

#include "queryresult.h"
#include "schema.h"

#include <QObject>

class QUrl;

/**
 * Abstract base class for database adapters
 */
class DatabaseAdapter : public QObject {
    Q_OBJECT
public:
    /**
     * Create a new database connection
     *
     * @param url
     * @return adapter of the proper subclass or null if protocol was unsupported
     */
    static DatabaseAdapter *openConnection(const QUrl &url, QObject *parent=nullptr);

    /**
     * Return the URL the database connection was opened with
     */
    QUrl url() const { return m_url; }

    /**
     * Get the name of the currently open database
     */
    virtual QString name() const = 0;

    /**
     * Check if the database connection is open
     */
    virtual bool isOpen() const = 0;

    /**
     * Return the last error message related to the database
     *
     * (Check this if isOpen() returns false.)
     */
    virtual QString databaseError() const = 0;

    /**
     * Execute a database query.
     *
     * The results of the query will be available asynchronously.
     * If the query result contains more rows than maxRows, only
     * maxRows will be returned, but queryMore can be called to ask for
     * more.
     *
     * @param query the query string to execute
     * @param maxRows maximum number of rows to return in one go
     * @return query ID
     */
    virtual QueryResult *query(const QString &query, int maxRows) = 0;


    //! Get the current database schema
    const dbschema::Database schema() const { return m_schema; }

public Q_SLOTS:
    /**
     * Query the database's schema.
     *
     * Once the query is complete, schemaRefreshed will be emitted.
     */
    virtual void refreshSchema() = 0;

    /**
     * Get a list of other databases available on this server
     *
     * Once the query is complete, otherDatabases() will be emitted.
     */
    virtual void refreshOtherDatabases() = 0;

Q_SIGNALS:
    void schemaRefreshed(const dbschema::Database &schema);
    void otherDatabases(const QStringList &databases, const QString &current);

protected:
    DatabaseAdapter(const QUrl &url, QObject *parent);

    /// Return a new query ID
    QueryId nextQueryId();

    dbschema::Database m_schema;

private:
    QUrl m_url;
    QueryId m_nextQueryId;
};

#endif
