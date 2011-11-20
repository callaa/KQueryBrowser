#include <QDebug>
#include <QWebView>
#include <QWebFrame>
#include <QVBoxLayout>
#include <QWebElement>

#include <KStandardDirs>
#include <KLineEdit>

#include "querywidget.h"
#include "queryjsobject.h"
#include "queryresults.h"

QueryWidget::QueryWidget(QWidget *parent) :
    QWidget(parent)
{
	// The web view is used to show the results of the queries
	m_view = new QWebView(this);
	m_view->setContextMenuPolicy(Qt::PreventContextMenu);
	connect(m_view, SIGNAL(loadFinished(bool)), this, SLOT(initQueryBrowser(bool)));

	// The query entry box
	m_query = new KLineEdit(this);
	connect(m_query, SIGNAL(returnPressed()), this, SLOT(doQuery()));

	// Build initial content for the web view
	//QString style = KStandardDirs::installPath("data") + "kquerybrowser/querystyle.css";
	QString style = "/home/calle/src/kquerybrowser/data/querystyle.css";
	QString script = "/home/calle/src/kquerybrowser/data/queryscript.js";

	qDebug() << "style:" << style;
	m_view->setHtml("<html><head><link href=\"file://" + style + "\" rel=\"stylesheet\"><script type=\"text/javascript\" src=\"file://" + script + "\"></script><title>KQueryBrowser</title></head><body></body></html>");
	m_view->page()->currentFrame()->addToJavaScriptWindowObject("qbrowser", new QueryJsObject(this));

	// Finish up.
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(m_view);
	layout->addWidget(m_query);

	setLayout(layout);

}

void QueryWidget::initQueryBrowser(bool ok)
{
	if(!ok)
		qDebug() << "Query browser load error!";

	m_view->page()->currentFrame()->evaluateJavaScript("initQueryBrowser()");
}

void QueryWidget::doQuery()
{
	QString q = m_query->text().trimmed();
	if(q.isEmpty())
		return;

	QWebFrame *w = m_view->page()->currentFrame();
	w->findFirstElement("body").appendInside("<div class=\"query\"><p class=\"query\"><p class=\"wait\"></p></div>");
	QWebElement qe = w->findFirstElement("div.query:last-child>p.query");
	qe.setPlainText(q);

	emit doQuery(m_query->text(), 0);
	m_query->setText(QString());
}

static void makeTable(QWebElement parent, const QVector<Column> &columns, const ResultRows &rows)
{
	// TODO do this more efficiently and escape HTML
	QString table = "<table><thead><tr>";
	foreach(const Column& c, columns) {
		table.append("<th>");
		table.append(c.name());
		table.append("</th>");
	}
	table.append("</tr></thead><tbody>");
	foreach(const ResultRow row, rows) {
		table.append("<tr>");
		foreach(const QVariant& c, row) {
			table.append("<td>");
			table.append(c.toString());
			table.append("</td>");
		}
		table.append("</tr>");
	}
	table.append("</tbody></table>");
	parent.appendInside(table);
}

void QueryWidget::queryResults(const QueryResults &results)
{
	QWebFrame *w = m_view->page()->currentFrame();
	w->findFirstElement(".wait").removeFromDocument();
	if(results.success()) {
		if(results.isSelect()) {
			// We got results. Format them in a table nicely.
			makeTable(w->findFirstElement("div.query:last-child"), results.columns(), results.rows());
		} else {
			// A non-select statement was executed succesfully
			w->findFirstElement("div.query:last-child").appendInside("<p class=\"success\">" + tr("Success. %1 rows affected.").arg(results.rowCount()) + "</p>");
		}
	} else {
		// An error occurred
		w->findFirstElement("div.query:last-child").appendInside("<p class=\"error\">Error:" + results.error() + "</p>");
	}
}
