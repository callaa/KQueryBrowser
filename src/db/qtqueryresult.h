#ifndef QTQUERYRESULT_H
#define QTQUERYRESULT_H

#include "queryresult.h"

#include <QSqlQuery>
#include <QJsonArray>

/**
 * A generic Query Result implementation for connections that use Qt SQL.
 */
class QtQueryResult : public QueryResult
{
public:
    QtQueryResult(QSqlQuery &&query, QObject *parent);

    State state() const override;
    QList<QueryResultColumn> columns() const override { return m_columns; }
    QJsonArray rows() const override { return m_rows; }
    int total() const override { return m_total; }
    int affectedRows() const override;
    State fetchMore(int rows) override;
    QString errorMessage() const override;

private:
    QSqlQuery m_query;
    QList<QueryResultColumn> m_columns;
    QJsonArray m_rows;
    int m_total;
};

#endif // QTQUERYRESULT_H
