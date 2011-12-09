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
#include <QTreeWidget>
#include <QHeaderView>
#include <QMenu>

#include "tablelistwidget.h"
#include "meta/database.h"

TableListWidget::TableListWidget(bool canshowcreate, QWidget *parent) :
	QDockWidget(tr("Tables"), parent), m_canshowcreate(canshowcreate)
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
	QIcon iconUnique(":/icons/unique.png");

	// Before clearing the old tree, remember which schemas/tables the
	// user had expanded.
	QStringList openschema, opentable;
	for(int i=0;i<m_view->topLevelItemCount();++i) {
		const QTreeWidgetItem *ti = m_view->topLevelItem(i);
		if(ti->data(0, Qt::UserRole)==0) {
			if(ti->isExpanded()) {
				openschema << ti->text(0);
				QString prefix = ti->text(0) + ".";
				for(int j=0;j<ti->childCount();++j) {
					QTreeWidgetItem *c = ti->child(j);
					if(c->isExpanded())
						opentable << prefix + c->text(0);
				}
			}
		} else {
			if(ti->isExpanded())
				opentable << ti->text(0);
		}
	}

	// Clear the old tree
	m_view->clear();

	// Build the new tree
	const bool hasSchemas = !db.noSchemas();
	foreach(const Schema& schema, db.schemas()) {
		QTreeWidgetItem *s=0;

		// If the DBMS doesn't have schemas, show tables as
		// the top level items.
		if(hasSchemas) {
			s = new QTreeWidgetItem(m_view);
			s->setText(0, schema.name());
			s->setData(0, Qt::UserRole, 0);
		}

		foreach(const Table& table, schema.tables()) {
			QTreeWidgetItem *t = s!=0 ? new QTreeWidgetItem(s) : new QTreeWidgetItem(m_view);
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
				else if(col.isUnique())
					c->setIcon(0, iconUnique);
				QString ttip = col.type();
				if(col.hasForeignKey()) {
					ttip = ttip + " " + col.foreignkey().toString();
				}

				c->setToolTip(0, ttip);
			}
		}
	}

	// Re-open the schemas/tables that had been previously open
	for(int i=0;i<m_view->topLevelItemCount();++i) {
		QTreeWidgetItem *ti = m_view->topLevelItem(i);
		if(ti->data(0, Qt::UserRole)==0) {
			if(openschema.contains(ti->text(0))) {
				ti->setExpanded(true);
				QString prefix = ti->text(0) + ".";
				for(int j=0;j<ti->childCount();++j) {
					QTreeWidgetItem *c = ti->child(j);
					if(opentable.contains(prefix + c->text(0)))
						c->setExpanded(true);
				}
			}
		} else {
			if(opentable.contains(ti->text(0)))
				ti->setExpanded(true);
		}
	}
}

void TableListWidget::customContextMenu(const QPoint& point)
{
	QTreeWidgetItem *item = m_view->itemAt(point);
	QMenu menu;

	QAction *showcreate=0;

	if(item!=0 && item->data(0, Qt::UserRole)!=0) {
		QString table;
		if(item->data(0, Qt::UserRole)==1) {
			QTreeWidgetItem *schema = item->parent();
			if(schema!=0)
				table = schema->text(0) + "." + item->text(0);
			else
				table = item->text(0);
		} else {
			QTreeWidgetItem *tbl = item->parent();
			QTreeWidgetItem *schema = tbl->parent();
			if(schema!=0)
				table = schema->text(0) + "." + tbl->text(0);
			else
				table = tbl->text(0);
		}

		menu.addAction("SELECT * FROM " + table);
		menu.addAction("SELECT COUNT(*) FROM " + table);

		if(item->data(0, Qt::UserRole)==2) {
			// TODO make these customizable
			menu.addAction("SELECT " + item->text(0) + " FROM " + table);
			menu.addAction("SELECT DISTINCT " + item->text(0) + " FROM " + table);
		}

		if(m_canshowcreate) {
			menu.addSeparator();
			showcreate = menu.addAction(tr("Show create script"));
			showcreate->setProperty("tablename", table);
		}

	}

	if(!menu.isEmpty())
		menu.addSeparator();
	QAction *refreshAct = menu.addAction("Refresh");

	QAction *a = menu.exec(m_view->mapToGlobal(point));
	if(a!=0) {
		if(a==refreshAct)
			emit refresh();
		else if(a==showcreate)
			emit showCreate(a->property("tablename").toString());
		else
			emit runQuery(a->text());
	}
}
