#include <QDebug>
#include <QWebFrame>
#include <QWebElement>

#include <KStandardDirs>

#include "queryview.h"
#include "queryresults.h"

//#include "queryjsobject.h"

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
	//page()->currentFrame()->addToJavaScriptWindowObject("qbrowser", new QueryJsObject(this));

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

	page()->currentFrame()->evaluateJavaScript("qb_newquery(\"query-" + QString::number(m_querycount) + "\")");
}

static void makeTable(int id, QWebElement parent, const QVector<Column> &columns, const ResultRows &rows)
{
	QStringList html;

	const QString TH("<th>");
	const QString eTH("</th>");
	const QString TD("<td>");
	const QString eTD("</dh>");
	const QString TR("<tr>");
	const QString eTR("</tr>");

	html << QString("<table id=\"query-%1\"><thead><tr>").arg(id);
	foreach(const Column& c, columns) {
		html << TH << c.name() << eTH;
	}
	html << "</tr></thead><tbody>";

	foreach(const ResultRow row, rows) {
		html << TR;
		foreach(const QVariant& c, row) {
			html << TD << c.toString() << eTD;
		}
		html << eTR;
	}
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
			// We got results. Format them in a table nicely.
			makeTable(++m_querycount, w->findFirstElement("div.query:last-child"), results.columns(), results.rows());
		} else {
			// A non-select statement was executed succesfully
			w->findFirstElement("div.query:last-child").appendInside("<p class=\"success\">" + tr("Success. %1 rows affected.").arg(results.rowCount()) + "</p>");
		}
	} else {
		// An error occurred
		w->findFirstElement("div.query:last-child").appendInside("<p class=\"error\">Error:" + results.error() + "</p>");
	}

	page()->currentFrame()->evaluateJavaScript("qb_endquery(\"query-" + QString::number(m_querycount) + "\")");
}
