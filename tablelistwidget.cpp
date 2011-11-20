#include <QDebug>
#include <QTreeWidget>
#include <QHeaderView>

#include "tablelistwidget.h"
#include "database.h"

TableListWidget::TableListWidget(QWidget *parent) :
	QDockWidget(tr("Tables"), parent)
{
	m_view = new QTreeWidget(this);
	setWidget(m_view);
	m_view->header()->hide();
}

void TableListWidget::refreshTree(const Database &db)
{
	m_view->clear();
	foreach(const Table& table, db.tables()) {
		QTreeWidgetItem *t = new QTreeWidgetItem(m_view);
		t->setText(0, table.name());

		foreach(const Column &col, table.columns()) {
			QTreeWidgetItem *c = new QTreeWidgetItem(t);
			c->setText(0, col.name());
		}
	}
}
