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
	QIcon iconTable(":/icons/table.png");
	QIcon iconView(":/icons/view.png");
	QIcon iconSys(":/icons/systable.png");
	QIcon iconKey(":/icons/key.png");

	m_view->clear();
	foreach(const Table& table, db.tables()) {
		QTreeWidgetItem *t = new QTreeWidgetItem(m_view);
		t->setText(0, table.name());
		switch(table.type()) {
			case Table::TABLE: t->setIcon(0, iconTable); break;
			case Table::VIEW: t->setIcon(0, iconView); break;
			case Table::SYSTEMTABLE: t->setIcon(0, iconSys); break;
		}

		foreach(const Column &col, table.columns()) {
			QTreeWidgetItem *c = new QTreeWidgetItem(t);
			c->setText(0, col.name());
			if(col.isPrimaryKey())
				c->setIcon(0, iconKey);
			c->setToolTip(0, col.type());
		}
	}
}
