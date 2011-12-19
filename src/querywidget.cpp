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
#include <QVBoxLayout>

#include <KGlobal>
#include <KConfigGroup>

#include "querywidget.h"
#include "queryview.h"
#include "sqllineedit.h"
#include "db/queryresults.h"
#include "ui_findwidget.h"

QueryWidget::QueryWidget(QWidget *parent) :
	QWidget(parent), m_moreavailable(false)
{
	// The web view is used to show the results of the queries
	m_view = new QueryView(this);
	connect(m_view, SIGNAL(getMoreResults(int)), this, SIGNAL(getMoreResults(int)));

	// The query entry box
	m_query = new SqlLineEdit(this);
	connect(m_query, SIGNAL(returnPressed(QString)), this, SLOT(doQuery(QString)));

	// Find in results widget
	m_find = new QWidget(this);
	m_findui = new Ui::FindWidget;
	m_findui->setupUi(m_find);
	m_find->hide();

	connect(m_findui->findtext, SIGNAL(textChanged(QString)),
			this, SLOT(findNext()));
	connect(m_findui->highlightall, SIGNAL(toggled(bool)),
			this, SLOT(findNext()));
	connect(m_findui->casesensitive, SIGNAL(toggled(bool)),
			this, SLOT(findNext()));

	m_findui->closebutton->setIcon(KIcon("dialog-close"));
	connect(m_findui->closebutton, SIGNAL(clicked(bool)),
			m_find, SLOT(hide()));

	m_findui->prevbutton->setIcon(KIcon("go-previous"));
	connect(m_findui->nextbutton, SIGNAL(clicked(bool)),
			this, SLOT(findNext()));
	m_findui->nextbutton->setIcon(KIcon("go-next"));
	connect(m_findui->prevbutton, SIGNAL(clicked(bool)),
			this, SLOT(findPrev()));

	// Finish up.
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(m_view);
	layout->addWidget(m_find);
	layout->addWidget(m_query);

	setLayout(layout);
}

void QueryWidget::showSearch()
{
	m_find->show();
	m_findui->findtext->setFocus();
	m_findui->findtext->selectAll();
}

TableCellIterator *QueryWidget::tableIterator() const
{
	return m_view->tableIterator();
}

void QueryWidget::runQuery(const QString &query)
{
	m_query->pushHistory(query);
	m_view->startNewQuery(query);
	int limit = KGlobal::config()->group("view").readEntry("resultsperpage", QueryView::DEFAULT_PAGESIZE);
	emit doQuery(query, limit);
}

void QueryWidget::doQuery(const QString& q)
{
	if(q.isEmpty()) {
		if(m_moreavailable)
			emit getMoreResults(10);
	} else {
		m_view->startNewQuery(q);
		int limit = KGlobal::config()->group("view").readEntry("resultsperpage", QueryView::DEFAULT_PAGESIZE);
		emit doQuery(q, limit);
	}
}

void QueryWidget::findNext()
{
	findInPage(true);
}

void QueryWidget::findPrev()
{
	findInPage(false);
}

void QueryWidget::findInPage(bool forward)
{
	if(!m_find->isVisible())
		showSearch();

	QWebPage::FindFlags flags = QWebPage::FindWrapsAroundDocument;
	if(m_findui->casesensitive->isChecked())
		flags |= QWebPage::FindCaseSensitively;
	if(m_findui->highlightall->isChecked())
		flags |= QWebPage::HighlightAllOccurrences;
	if(!forward)
		flags |= QWebPage::FindBackward;

	if(m_view->findText(m_findui->findtext->text(), flags)) {
		// Found text
	} else {
		// Not found
	}
}

void QueryWidget::queryResults(const QueryResults &results)
{
	m_moreavailable = results.isMore();
	m_view->showResults(results);
}

void QueryWidget::clearResults()
{
	m_view->clear();
}

void QueryWidget::showEvent(QShowEvent *e)
{
	m_query->setFocus();
	QWidget::showEvent(e);
}
