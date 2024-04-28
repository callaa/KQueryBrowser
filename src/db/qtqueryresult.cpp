#include "qtqueryresult.h"

#include <QSqlError>
#include <QSqlRecord>
#include <QSqlField>
#include <QJsonObject>
#include <KLocalizedString>
#include <QSqlResult>

using namespace Qt::StringLiterals;

QtQueryResult::QtQueryResult(QSqlQuery &&query, QObject *parent)
    : QueryResult(query.lastQuery(), parent), m_query{std::move(query)}, m_total(0)
{
}

QueryResult::State QtQueryResult::state() const
{
    if(m_query.lastError().isValid()) {
        return State::Error;
    }
    if(m_query.isValid()) {
        return State::HasMoreResults;
    }
    return State::Done;
}

QueryResult::State QtQueryResult::fetchMore(int maxRows)
{
    if(!m_query.isValid()) {
        return state();
    }

    // Set column header if not yet set
    if(m_columns.isEmpty()) {
        const int cols = m_query.record().count();
        m_columns.reserve(cols);
        for(int i=0;i<cols;++i) {
            QSqlField f = m_query.record().field(i);
            m_columns.append({
                f.name(),
                QString::fromUtf8(f.metaType().name()),
            });
        }
    }

    // Fetch rows up to the requested maximum
    m_rows = QJsonArray{};
    do {
        QJsonArray row;

        for(int i=0;i<m_columns.length();++i) {
            const auto val = m_query.value(i);
            if(val.typeId() == QMetaType::QByteArray) {
                // Binary data
                QJsonObject o{
                    {u"type"_s, u"binary"_s},
                    {u"data"_s, QString::fromLatin1(val.toByteArray().toBase64())},
                };
                row.append(o);

            } else {
                row.append(QJsonValue::fromVariant(val));
            }
        }
        m_rows.append(row);
        ++m_total;
    } while(m_query.next() && (maxRows <=0 || m_rows.size() < maxRows));

    // QSqlQuery is synchronous so we always know the final state here, no need to emit resultsAvailable
    return m_query.isValid() ? State::HasMoreResults : State::Done;
}

int QtQueryResult::affectedRows() const
{
    return m_query.numRowsAffected();
}

QString QtQueryResult::errorMessage() const
{
    QString error;
    switch(m_query.lastError().type()) {
    case QSqlError::NoError: return QString();
    case QSqlError::ConnectionError: error = i18n("Connection error"); break;
    case QSqlError::StatementError: error = i18n("Syntax error"); break;
    case QSqlError::TransactionError: error = i18n("Transaction error"); break;
    default: error = i18n("Unknown error"); break;
    }

    return u"%1: %2"_s.arg(error, m_query.lastError().text());
}
