#include <QDebug>
#include <QTreeWidget>
#include <QHeaderView>
#include <QMenu>

#include "tablelistwidget.h"
#include "database.h"

TableListWidget::TableListWidget(QWidget *parent) :
	QDockWidget(tr("Tables"), parent)
{
	m_view = new QTreeWidget(this);
	m_view->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

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
		t->setData(0, Qt::UserRole, 1);

		foreach(const Column &col, table.columns()) {
			QTreeWidgetItem *c = new QTreeWidgetItem(t);
			c->setText(0, col.name());
			c->setData(0, Qt::UserRole, 2);
			if(col.isPrimaryKey())
				c->setIcon(0, iconKey);
			c->setToolTip(0, col.type());
		}
	}
}

void TableListWidget::customContextMenu(const QPoint& point)
{
	QTreeWidgetItem *item = m_view->itemAt(point);
	if(item!=0) {
		QString table;
		if(item->data(0, Qt::UserRole)==1)
			table = item->text(0);
		else
			table = item->parent()->text(0);

		QMenu menu;
		menu.addAction("SELECT * FROM " + table);
		menu.addAction("SELECT COUNT(*) FROM " + table);

		if(item->data(0, Qt::UserRole)==2) {
			// TODO make these customizable
			menu.addAction("SELECT " + item->text(0) + " FROM " + table);
			menu.addAction("SELECT DISTINCT " + item->text(0) + " FROM " + table);
		}

		QAction *a = menu.exec(m_view->mapToGlobal(point));
		if(a!=0)
			emit runQuery(a->text());
	}
}
