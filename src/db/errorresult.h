#ifndef ERRORRESULT_H
#define ERRORRESULT_H

#include "queryresult.h"

class ErrorResult : public QueryResult
{
public:
    ErrorResult(const QString &message, const QString &sql, QObject *parent=nullptr);

    State state() const override { return State::Error; }
    QList<QueryResultColumn> columns() const override { return QList<QueryResultColumn>();}
    QJsonArray rows() const override { return QJsonArray(); }
    int total() const override { return 0; }
    int affectedRows() const override { return 0; }
    State fetchMore(int rows) override { Q_UNUSED(rows); return State::Error; }
    QString errorMessage() const override { return m_message; }

private:
    QString m_message;
};

#endif // ERRORRESULT_H
