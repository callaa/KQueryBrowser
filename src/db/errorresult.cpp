#include "errorresult.h"

ErrorResult::ErrorResult(const QString &message, const QString &sql, QObject *parent)
    : QueryResult(sql, parent), m_message(message)
{
}
