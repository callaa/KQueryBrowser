#include "queryview.h"
#include "webapi.h"
#include "db/database.h"
#include "widgets/sqllineedit.h"
#include "widgets/findwidget.h"
#include "utils/sqlcompletion.h"

#include <QDebug>
#include <QWebEngineView>
#include <QVBoxLayout>
#include <QJsonObject>
#include <QJsonDocument>
#include <KCompletion>

using namespace Qt::StringLiterals;

int QueryView::VIEWS = 0;

QueryView::QueryView(DatabaseAdapter *db, WebApi *webapi, QWebEngineProfile *profile, bool showInputLine, QWidget *parent)
    : QWidget(parent), m_db(db), m_currentResult(nullptr), m_firstResult(true), m_viewId(++VIEWS)
{
    Q_ASSERT(db);
    Q_ASSERT(webapi);
    Q_ASSERT(profile);

    auto layout = new QVBoxLayout(this);

    // A web engine view to render the results
	m_view = new QWebEngineView(profile, this);
    m_view->setUrl(QUrl(QStringLiteral("qrc:/html/query.html")));
    layout->addWidget(m_view, 1);

    // Finder widget (hidden by default)
    m_finder = new FindWidget(this);
    m_finder->hide(); // finder is hidden until explicitly opened
    connect(m_finder, &FindWidget::findRequested, this, [this](const QString &text, bool next, bool caseSensitive) {
        QWebEnginePage::FindFlags flags;
        if(!next)
            flags.setFlag(QWebEnginePage::FindBackward);
        if(caseSensitive)
            flags.setFlag(QWebEnginePage::FindCaseSensitively);

        m_view->findText(text, flags);
    });
    layout->addWidget(m_finder);

    // Query input line
    if(showInputLine) {
        m_line = new SqlLineEdit(this);
        layout->addWidget(m_line);
        connect(m_line, &SqlLineEdit::returnPressed, this, &QueryView::makeQuery);

        // Autocompletion for query input
        SqlCompletion *c = new SqlCompletion;
        c->setIgnoreCase(true);
        c->refreshModel(db->schema());
        connect(db, &DatabaseAdapter::schemaRefreshed, c, &SqlCompletion::refreshModel);
        m_line->setCompletionObject(c);
        m_line->setCompletionMode(KCompletion::CompletionAuto);
    } else {
        m_line = nullptr;
    }

    // This function is used by the page to poll for new results to show
    // (polling is triggered by us calling the pollResults function)
    webapi->registerMethod(u"getResults/%1"_s.arg(m_viewId), this, "getResults(QJsonObject)");

    // Action button calls
    webapi->registerMethod(u"fetchMoreResults/%1"_s.arg(m_viewId), this, "fetchMoreResults(QJsonObject)");
    webapi->registerMethod(u"stopResults/%1"_s.arg(m_viewId), this, "stopResults(QJsonObject)");

	setLayout(layout);
}

QueryView::~QueryView()
{
    delete m_currentResult;
}

void QueryView::find()
{
    m_finder->showFinder();
}

void QueryView::findNext()
{
    m_finder->findNext();
}

void QueryView::findPrev()
{
    m_finder->findPrev();
}

void QueryView::makeQuery(const QString &query)
{
    if(m_currentResult) {
        stopResults();
    }

    QJsonObject newQueryParams{
        {u"sql"_s, query}
    };

    m_view->page()->runJavaScript(u"newQuery(%1)"_s.arg(QString::fromUtf8(QJsonDocument{newQueryParams}.toJson(QJsonDocument::Compact))));

    m_currentResult = m_db->query(query, 50);

    // Let the web view UI know there is more data to fetch
    connect(m_currentResult, &QueryResult::resultsAvailable, this, &QueryView::notifyNewResults);
    if(m_currentResult->state() != QueryResult::State::Awaiting) {
        notifyNewResults();
    }
}

void QueryView::notifyNewResults()
{
    m_view->page()->runJavaScript(u"pollResults(%1)"_s.arg(m_viewId));
}

QJsonDocument QueryView::fetchMoreResults(const QJsonObject &kwargs)
{
    if(!m_currentResult) {
        qWarning() << "fetchMoreResults called but there is no active query!";
        return QJsonDocument();
    }

    const int maxRows = kwargs[u"maxRows"_s].toInt(50);
    if(m_currentResult->fetchMore(maxRows) != QueryResult::State::Awaiting) {
        // results immediately available
        notifyNewResults();
    }

    return QJsonDocument();
}

QJsonDocument QueryView::stopResults(const QJsonObject &kwargs)
{
    Q_UNUSED(kwargs);
    stopResults();
    return QJsonDocument();
}

void QueryView::clearResults()
{
    m_view->page()->runJavaScript(u"clearResults()"_s);
}

/**
 * Web API: return available results
 *
 * Result object can be one of:
 *
 * In case of error:
 *   {
 *     "error": "..."
 *   }
 *
 * If still awaiting for new data:
 *   {
 *   }
 *
 * When data is available:
 *   {
 *     "more": true | false,
 *     "total": running tally of rows fetched,
 *     "affected": number of rows affected by the query,
 *     "columns": [
 *       {
 *         "name": "name",
 *         "type": "type"
 *     ],
 *     "rows": [
 *       [...]
 *     ]
 *   }
 */
QJsonDocument QueryView::getResults(const QJsonObject &kwargs)
{
    Q_UNUSED(kwargs);

    QJsonObject ret;

    const auto state = m_currentResult->state();

    if(state == QueryResult::State::Awaiting) {
        qDebug() << "still waiting for new results...";

    } else if(state == QueryResult::State::Error) {
        ret[u"error"_s] = m_currentResult->errorMessage();

    } else {
        ret[u"more"_s] = state == QueryResult::State::HasMoreResults;
        ret[u"rows"_s] = m_currentResult->rows();
        ret[u"total"_s] = m_currentResult->total();
        ret[u"affected"_s] = m_currentResult->affectedRows();

        // Columns are not passed on continuation results
        if(m_firstResult) {
            m_firstResult = false;
            QJsonArray columns;
            for(const auto &col : m_currentResult->columns()) {
                columns << QJsonObject{
                    {u"name"_s, col.name},
                    {u"type"_s, col.type},
                    };
            }
            ret[u"columns"_s] = columns;
        }
    }

    return QJsonDocument(ret);
}

void QueryView::stopResults()
{
    m_view->page()->runJavaScript(u"stopResults()"_s);
    delete m_currentResult;
    m_currentResult = nullptr;
    m_firstResult = true;
}
