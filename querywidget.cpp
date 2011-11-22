#include <QDebug>
#include <QVBoxLayout>

#include "querywidget.h"
#include "queryview.h"
#include "sqllineedit.h"

QueryWidget::QueryWidget(QWidget *parent) :
	QWidget(parent)
{
	// The web view is used to show the results of the queries
	m_view = new QueryView(this);

	// The query entry box
	m_query = new SqlLineEdit(this);
	connect(m_query, SIGNAL(returnPressed(QString)), this, SLOT(doQuery(QString)));

	// Finish up.
	QVBoxLayout *layout = new QVBoxLayout();
	layout->addWidget(m_view);
	layout->addWidget(m_query);

	setLayout(layout);
}

void QueryWidget::doQuery(const QString& q)
{
	m_view->startNewQuery(q);
	emit doQuery(q, 0);
}

void QueryWidget::queryResults(const QueryResults &results)
{
	m_view->showResults(results);
}
