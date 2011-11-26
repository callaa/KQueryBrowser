#include <QDebug>
#include <QVBoxLayout>

#include <KGlobal>
#include <KConfigGroup>

#include "querywidget.h"
#include "queryview.h"
#include "sqllineedit.h"
#include "queryresults.h"

QueryWidget::QueryWidget(QWidget *parent) :
	QWidget(parent), m_moreavailable(false)
{
	// The web view is used to show the results of the queries
	m_view = new QueryView(this);
	connect(m_view, SIGNAL(getMoreResults(int)), this, SIGNAL(getMoreResults(int)));

	// The query entry box
	m_query = new SqlLineEdit(this);
	connect(m_query, SIGNAL(returnPressed(QString)), this, SLOT(doQuery(QString)));

	// Finish up.
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(m_view);
	layout->addWidget(m_query);

	setLayout(layout);
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

void QueryWidget::queryResults(const QueryResults &results)
{
	m_moreavailable = results.isMore();
	m_view->showResults(results);
}

void QueryWidget::showEvent(QShowEvent *e)
{
	m_query->setFocus();
	QWidget::showEvent(e);
}
