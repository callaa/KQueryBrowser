#ifndef QUERYRESULT_H
#define QUERYRESULT_H

#include <QObject>
#include <QList>
#include <QJsonArray>

typedef int QueryId;

struct QueryResultColumn {
    QString name;
    QString type;
};

class QueryResult : public QObject
{
    Q_OBJECT
public:
    enum class State {
        Awaiting, // results are not yet ready
        HasMoreResults, // more results can still be fetched
        Done, // query complete, no more rows to fetch
        Error // query failed, check error message
    };

    /**
     * Get the query that was executed
     */
    QString sql() const { return m_sql; }

    /**
     * Get the state of the result
     */
    virtual State state() const = 0;

    /**
     * Get the columns of the result set
     */
    virtual QList<QueryResultColumn> columns() const = 0;

    /**
     * Return the currently available set of rows.
     *
     * Each row is a JSON array with each item corresponding to a column.
     * In addition to the primitive types, the following rich types may be included:
     *
     * Binary data:
     * {
     *   "type": "binary"
     *   "data": "base64 encoded binary data"
     * }
     *
     * Calling fetchMore will reset the row buffer.
     *
     * @return an array of arrays
     */
    virtual QJsonArray rows() const = 0;

    /**
     * Return the total number of rows fetched so far.
     *
     * Note that we can't reliably get the total size of the select result set
     * without fetching it entirely, so this is just a running tally.
     */
    virtual int total() const = 0;

    /**
     * Return the number of rows affected by the query.
     *
     * (Generally, an INSERT, UPDATE, or DELETE query)
     */
    virtual int affectedRows() const = 0;

    /**
     * Request more rows
     *
     * Note: calling this clears the current row list.
     * If returned state is Awaiting, resultsAvailable will be emitted once the
     * result set becomes ready.
     *
     * @param rows max number of rows to return (if zero or negative, request all remaining rows)
     * @return query result state
     */
    virtual State fetchMore(int rows) = 0;

    /**
     * Get the error message (if state() == Error)
     */
    virtual QString errorMessage() const = 0;

Q_SIGNALS:
    /**
     * Emitted when more results become available
     */
    void resultsAvailable();

protected:
    QueryResult(const QString &sql, QObject *parent);

private:
    QString m_sql;
};

#endif // QUERYRESULT_H
