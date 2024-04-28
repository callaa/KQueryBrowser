#include "queryresult.h"

QueryResult::QueryResult(const QString &sql, QObject *parent)
    : QObject(parent), m_sql(sql)
{
}
