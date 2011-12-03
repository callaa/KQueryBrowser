//
// This file is part of KQueryBrowser.
//
// KQueryBrowser is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KQueryBrowser is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KQueryBrowser.  If not, see <http://www.gnu.org/licenses/>.
//
#include <QDebug>
#include <QWebFrame>
#include <QWebElement>
#include <QBuffer>

#include <KStandardDirs>
#include <KConfig>
#include <KConfigGroup>
#include <KMessageBox>
#include <KEncodingFileDialog>
#include <KSaveFile>

#include "queryview.h"
#include "db/queryresults.h"
#include "export/exporter.h"
#include "valueview.h"
#include "stringbuilder.h"

const int QueryView::DEFAULT_PAGESIZE;

class HtmlTableIterator : public TableCellIterator
{
public:
	HtmlTableIterator(const QWebFrame *frame, const QVector<BigValue> &bigvalues)
		: m_frame(frame), m_bigvalues(bigvalues)
	{ }

	int columns() const { return m_cheaders.count(); }
	int rows() const { return m_crows; }
	const Column& header(int column) const { return m_cheaders.at(column); }
	const QString& query() const { return m_query; }

	bool nextColumn();
	bool nextRow();
	bool nextTable();

	// Internal: get ID of current query
	const QString queryid() const { return m_queryid; }

	QVariant value() const;

private:
	const QWebFrame *m_frame;
	const QVector<BigValue> &m_bigvalues;
	QString m_query;
	QString m_queryid;
	int m_crows;
	QVector<Column> m_cheaders;
	QWebElement m_el;
};

QueryView::QueryView(QWidget *parent) :
	QWebView(parent), m_querycount(0)
{
	setContextMenuPolicy(Qt::PreventContextMenu);
	connect(this, SIGNAL(loadFinished(bool)), this, SLOT(initQueryBrowser(bool)));

	// Build initial content for the web view
	QString root = KStandardDirs::installPath("data") + "kquerybrowser/";
	QString style = root + "querystyle.css";
	QString script = root + "queryscript.js";

	setHtml("<html><head><link href=\"file://" + style + "\" rel=\"stylesheet\"><script type=\"text/javascript\" src=\"file://" + script + "\"></script><title>KQueryBrowser</title></head><body></body></html>");

	page()->currentFrame()->addToJavaScriptWindowObject("qbrowser", this);

}

void QueryView::initQueryBrowser(bool ok)
{
	if(!ok)
		qWarning("Query browser load error!");
	else {
		StringBuilder sb;
		sb << "qb_init([";
		foreach(ExporterFactory *ef, Exporters::instance().exporters()) {
			sb << "{format: '" << ef->format() << "'";
			if(!ef->icon().isNull()) {
				sb << ", icon: 'data:image/png;base64,";
				QBuffer img;
				img.open(QBuffer::WriteOnly);
				ef->icon().pixmap(24,24).save(&img, "PNG");
				sb << img.data().toBase64();
				sb << "'";
			}
			sb << "},";
		}
		sb << "])";

		page()->currentFrame()->evaluateJavaScript(sb.toString());
	}
}

void QueryView::clear()
{
	m_bigresults.clear();
	foreach(QWebElement e, page()->currentFrame()->findAllElements("div.query"))
		e.removeFromDocument();
}

TableCellIterator *QueryView::tableIterator() const
{
	return new HtmlTableIterator(page()->currentFrame(), m_bigresults);
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

static void makeTable(QWebElement parent, const QVector<Column> &columns, const ResultRows &rows, bool newtable, QVector<BigValue> &bigresults, QStringList &columnnames)
{
	StringBuilder html;

	const QString TD("<td>");
	const QString eTD("</dh>");
	const QString TR("<tr>");
	const QString eTR("</tr>\n");
	const QString NULLVAL("<b>NULL</b>");
	const QString BIGLINK_("<a data-index=\"");
	const QString _BIGLINK("\" onclick=\"qb_show(this)\" href=\"#\">");
	const QString eA("</a>");

	if(newtable) {
		const QString TH("<th>");
		const QString eTH("</th>");
		html << "<table><thead><tr>";
		columnnames.clear();
		foreach(const Column& c, columns) {
			html << TH << esc(c.name()) << eTH;
			columnnames << c.name();
		}
		html << "</tr></thead><tbody>\n";
	}

	foreach(const ResultRow row, rows) {
		html << TR;
		int col=0;
		foreach(const QVariant& c, row) {
			if(c.isNull()) {
				html << TD << NULLVAL << eTD;
			} else {
				QString val = c.toString();
				if(val.length() > 40) {
					// TODO configurable limit
					bigresults.append(BigValue(columnnames.at(col),c));
					val.truncate(40);
					html << TD << BIGLINK_ <<
						QString::number(bigresults.count()-1) <<
						_BIGLINK << esc(val) << eA << eTD;
				} else {
					html << TD << esc(val) << eTD;
				}
			}
			++col;
		}
		html << eTR;
	}

	if(newtable)
		html << "</tbody></table>";

	parent.appendInside(html.toString());

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
				makeTable(w->findFirstElement("div.query:last-child>table>tbody"), results.columns(), results.rows(), false, m_bigresults, m_columnnames);
			} else {
				m_resultsexpected = results.rowCount();
				makeTable(w->findFirstElement("div.query:last-child"), results.columns(), results.rows(), true, m_bigresults, m_columnnames);
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

void QueryView::showBigResult(int index)
{
	if(index<0 || index>= m_bigresults.count()) {
		qWarning("Big result index %d out of range [0..%d]", index, m_bigresults.count()-1);
		return;
	}

	const BigValue &bv = m_bigresults.at(index);
	ValueView *view = new ValueView(bv.second, this);
	view->setCaption(bv.first);
	view->show();
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

void QueryView::exportTable(const QString& id, const QString& format)
{
	KEncodingFileDialog::Result filename = KEncodingFileDialog::getSaveFileNameAndEncoding(
			"UTF-8",
			QString(),
			"*." + Exporters::instance().getExtension(format) + "|" +
			format + "\n*|All files",
			this);
	
	if(!filename.fileNames.at(0).isEmpty()) {
		KSaveFile file(filename.fileNames.at(0));
		if(!file.open()) {
			KMessageBox::error(this, file.errorString());
		} else {
			HtmlTableIterator iterator(page()->currentFrame(), m_bigresults);

			Exporter *exporter = Exporters::instance().get(format);

			exporter->startFile(&file, filename.encoding);

			// Find the table and export just that one
			while(iterator.nextTable()) {
				if(iterator.queryid() == id) {
					exporter->beginTable(&iterator);
					break;
				}
			}
			exporter->done();
			delete exporter;

			if(!file.finalize())
				KMessageBox::error(this, file.errorString());
			file.close();
		}
	}

}

bool HtmlTableIterator::nextTable()
{
	// Find next query div
	if(m_el.isNull()) {
		// First call
		m_el = m_frame->findFirstElement("div.query");
	} else {
		// We've already iterated over the query div, back up and go to the next
		m_el = m_el.nextSibling();
		while(!m_el.isNull() && (m_el.tagName() != "DIV" || !m_el.hasClass("query")))
			m_el = m_el.nextSibling();
	}

	if(m_el.isNull())
		return false;

	// Get headers
	QWebElementCollection headers = m_el.findAll("thead th");
	if(headers.count()==0)
		return false;

	m_cheaders.clear();
	for(int i=0;i<headers.count();++i)
		m_cheaders.append(Column(headers.at(i).toPlainText()));

	// Get query string
	m_query = m_el.findFirst("p.query").toPlainText();

	// Get query ID
	m_queryid = m_el.attribute("id");

	return true;
}

bool HtmlTableIterator::nextRow()
{
	QWebElement old = m_el;
	// If current tag name is TR, we got (back) here by nextCell().
	// Just move over to the next row.
	// If it is DIV, this is the first nextRow() call since nextTable().
	if(m_el.tagName() == "TR")
		m_el = m_el.nextSibling();
	else if(m_el.tagName() == "DIV")
		m_el = m_el.findFirst("tbody>tr");
	else {
		qWarning("Element pointer at a %s, expected tr or div!", m_el.tagName().toAscii().constData());
		return false;
	}

	// If element was not found, rewind back to query div
	if(m_el.isNull()) {
		m_el = old;
		while(!m_el.isNull() && (m_el.tagName() != "DIV" || !m_el.hasClass("query")))
			m_el = m_el.parent();
		return false;
	} else
		return true;
}

bool HtmlTableIterator::nextColumn()
{
	// If current tag name is TD, just move over to the next one.
	// If it is TR, this is the first call to nextColumn() since nextRow()
	if(m_el.tagName()=="TD") {
		QWebElement e = m_el;
		// Move to next cell, or parent tr if this was the last cell
		m_el = m_el.nextSibling();
		if(m_el.isNull()) {
			m_el = e.parent();
		}
	} else if(m_el.tagName()=="TR") {
		m_el = m_el.firstChild();
	} else {
		qWarning("Element pointer at a %s, expected td or tr!", m_el.tagName().toAscii().constData());
		return false;
	}

	return m_el.tagName()=="TD";
}

QVariant HtmlTableIterator::value() const
{
	QWebElement fc = m_el.firstChild();
	// we use "B" tags to indicate nulls
	if(fc.tagName()=="B")
		return QVariant(QVariant::String);
	else if(fc.tagName()=="A") {
		// A long value
		return m_bigvalues.at(fc.attribute("data-index").toInt()).second;
	} else
		return QVariant(m_el.toPlainText());
}

