#include <QDebug>
#include <QWebFrame>
#include <QWebElement>

#include <KStandardDirs>
#include <KConfig>
#include <KConfigGroup>

#include "queryview.h"
#include "queryresults.h"

const int QueryView::DEFAULT_PAGESIZE;

QueryView::QueryView(QWidget *parent) :
	QWebView(parent), m_querycount(0)
{
	setContextMenuPolicy(Qt::PreventContextMenu);
	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(initQueryBrowser(bool)));

	// Build initial content for the web view
	//QString style = KStandardDirs::installPath("data") + "kquerybrowser/querystyle.css";
	QString style = "/home/calle/src/kquerybrowser/data/querystyle.css";
	QString script = "/home/calle/src/kquerybrowser/data/queryscript.js";

	qDebug() << "style:" << style;
	setHtml("<html><head><link href=\"file://" + style + "\" rel=\"stylesheet\"><script type=\"text/javascript\" src=\"file://" + script + "\"></script><title>KQueryBrowser</title></head><body></body></html>");

	// TODO
	page()->currentFrame()->addToJavaScriptWindowObject("qbrowser", this);

}

void QueryView::initQueryBrowser(bool ok)
{
	if(!ok)
		qDebug() << "Query browser load error!";

	page()->currentFrame()->evaluateJavaScript("qb_init()");
}

void QueryView::clear()
{
	foreach(QWebElement e, page()->currentFrame()->findAllElements("div.query"))
		e.removeFromDocument();
}

void QueryView::startNewQuery(const QString &querystr)
{
	QWebFrame *w = page()->currentFrame();
	w->findFirstElement("body").appendInside("<div class=\"query\" id=\"query-" + QString::number(++m_querycount) + "\"><p class=\"query\"><p class=\"wait\"></p></div>");
	QWebElement qe = w->findFirstElement("div.query:last-child>p.query");
	qe.setPlainText(querystr);

	m_resultsgot = 0;
	page()->currentFrame()->evaluateJavaScript("qb_newquery(\"query-" + QString::number(m_querycount) + "\")");
}

static QString esc(QString text) {
	return text.replace('&', "&amp;").replace('<', "&lt;").replace('>', "&gt;");
}

static void makeTable(QWebElement parent, const QVector<Column> &columns, const ResultRows &rows, bool newtable)
{
	QStringList html;

	const QString TD("<td>");
	const QString eTD("</dh>");
	const QString TR("<tr>");
	const QString eTR("</tr>");

	if(newtable) {
		const QString TH("<th>");
		const QString eTH("</th>");
		html << "<table><thead><tr>";
		foreach(const Column& c, columns) {
			html << TH << esc(c.name()) << eTH;
		}
		html << "</tr></thead><tbody>";
	}

	foreach(const ResultRow row, rows) {
		html << TR;
		foreach(const QVariant& c, row) {
			html << TD << esc(c.toString()) << eTD;
		}
		html << eTR;
	}

	if(newtable)
		html << "</tbody></table>";

	QString table;
	int len=0;
	foreach(const QString q, html)
		len += q.length();
	table.reserve(len);
	foreach(const QString q, html)
		table.append(q);

	parent.appendInside(table);

}

void QueryView::showResults(const QueryResults &results)
{
	QWebFrame *w = page()->currentFrame();
	w->findFirstElement(".wait").removeFromDocument();
	if(results.success()) {
		if(results.isSelect()) {
			m_resultsgot += results.rows().count();
			// We got results. Format them in a table nicely.
			if(results.isContinuation()) {
				makeTable(w->findFirstElement("div.query:last-child>table>tbody"), results.columns(), results.rows(), false);
			} else {
				m_resultsexpected = results.rowCount();
				makeTable(w->findFirstElement("div.query:last-child"), results.columns(), results.rows(), true);
			}
		} else {
			// A non-select statement was executed succesfully
			w->findFirstElement("div.query:last-child").appendInside("<p class=\"success\">" + tr("Success. %1 rows affected.").arg(results.rowCount()) + "</p>");
		}
	} else {
		// An error occurred
		w->findFirstElement("div.query:last-child").appendInside("<p class=\"error\">" + results.error() + "</p>");
		m_resultsgot = -1;
	}

	if(results.isMore())
		page()->currentFrame()->evaluateJavaScript(QString("qb_partialquery(\"query-%1\", %2, %3)").arg(m_querycount).arg(m_resultsgot).arg(m_resultsexpected));
	else
		page()->currentFrame()->evaluateJavaScript(QString("qb_endquery(\"query-%1\", %2)").arg(m_querycount).arg(m_resultsgot));
}

void QueryView::queryGetMore()
{
	int limit = KGlobal::config()->group("view").readEntry("resultsperpage", DEFAULT_PAGESIZE);
	emit getMoreResults(limit);
}

void QueryView::queryGetAll()
{
	emit getMoreResults(-1);
}
