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
#include "tablelist.h"
#include "db/schema.h"

#include <QDebug>
#include <QTreeWidget>
#include <QHeaderView>
#include <QMenu>
#include <KLocalizedString>

using namespace Qt::StringLiterals;

static const int ROLE_SCHEMA = 0;
static const int ROLE_TABLE = 1;
static const int ROLE_COLUMN = 2;

TableListDock::TableListDock(QWidget *parent) :
    QDockWidget(i18n("Tables"), parent)
{
    //m_canshowcreate = connection->isCapable(db::Connection::SHOW_CREATE);
	m_view = new QTreeWidget(this);
	m_view->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_view, &QTreeWidget::customContextMenuRequested, this, &TableListDock::customContextMenu);

	setWidget(m_view);
	m_view->header()->hide();
}

void TableListDock::setSchema(const dbschema::Database &db)
{
	// Before clearing the old tree, remember which schemas/tables the
	// user had expanded.
	QStringList openschema, opentable;
	for(int i=0;i<m_view->topLevelItemCount();++i) {
		const QTreeWidgetItem *ti = m_view->topLevelItem(i);
        if(ti->data(0, Qt::UserRole)==ROLE_SCHEMA) {
			if(ti->isExpanded()) {
				openschema << ti->text(0);
                QString prefix = u"%1."_s.arg(ti->text(0));
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

    // Defaults
    if(openschema.isEmpty()) {
        openschema << u"main"_s << u"public"_s;
    }

	// Clear the old tree
	m_view->clear();

    QFont primaryKeyFont;
    primaryKeyFont.setUnderline(true);

	// Build the new tree
    for(const dbschema::Schema &schema : db.schemas) {
        QTreeWidgetItem *s = nullptr;

		// If the DBMS doesn't have schemas, show tables as
		// the top level items.
        if(db.hasSchemas) {
			s = new QTreeWidgetItem(m_view);
            s->setText(0, schema.name);
            s->setData(0, Qt::UserRole, ROLE_SCHEMA);
		}

        for(const dbschema::Table &table : schema.tables) {
            QTreeWidgetItem *t = s ? new QTreeWidgetItem(s) : new QTreeWidgetItem(m_view);
            t->setText(0, table.name);
#if 0 // TODO
			switch(table.type()) {
            case dbschema::Table::Type::Table: t->setIcon(0, iconTable); break;
            case dbschema::Table::Type::View: t->setIcon(0, iconView); break;
            case dbschema::Table::Type::SystemTable: t->setIcon(0, iconSys); break;
			}
#endif
            t->setData(0, Qt::UserRole, ROLE_TABLE);

            for(const dbschema::Column &col : table.columns) {
				QTreeWidgetItem *c = new QTreeWidgetItem(t);
                c->setText(0, col.name);
                c->setData(0, Qt::UserRole, ROLE_COLUMN);
                if(col.isPrimaryKey) {
                    c->setFont(0, primaryKeyFont);
                }

                if(col.isUnique) {
                    c->setIcon(0, QIcon::fromTheme(u"unique_constraint"_s));
                } else if(!col.foreignKeyToTable.isEmpty()) {
                    c->setIcon(0, QIcon::fromTheme(u"foreignkey_constraint"_s));
                }

                QString ttip = col.type;

                if(col.notNull) {
                    ttip += u" NOT NULL"_s;
                }
                if(!col.foreignKeyToTable.isEmpty()) {
                    ttip = ttip + u' ' + col.foreignKeyString();
				}

				c->setToolTip(0, ttip);
			}
		}
	}

	// Re-open the schemas/tables that had been previously open
	for(int i=0;i<m_view->topLevelItemCount();++i) {
		QTreeWidgetItem *ti = m_view->topLevelItem(i);
        if(ti->data(0, Qt::UserRole)==ROLE_SCHEMA) {
			if(openschema.contains(ti->text(0))) {
				ti->setExpanded(true);
                QString prefix = ti->text(0) + u'.';
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

void TableListDock::customContextMenu(const QPoint& point)
{
	QTreeWidgetItem *item = m_view->itemAt(point);
	QMenu menu;

#if 0 // TODO
    QAction *showcreate = nullptr;
#endif
    if(item && item->data(0, Qt::UserRole)!=ROLE_SCHEMA) {
		QString table;
        if(item->data(0, Qt::UserRole)==ROLE_TABLE) {
			QTreeWidgetItem *schema = item->parent();
            if(schema)
                table = u"%1.%2"_s.arg(schema->text(0), item->text(0));
			else
				table = item->text(0);

		} else {
			QTreeWidgetItem *tbl = item->parent();
			QTreeWidgetItem *schema = tbl->parent();
            if(schema)
                table = u"%1.%2"_s.arg(schema->text(0), tbl->text(0));
			else
				table = tbl->text(0);
		}

        menu.addAction(u"SELECT * FROM %1"_s.arg(table));
        menu.addAction(u"SELECT COUNT(*) FROM %1"_s.arg(table));

        if(item->data(0, Qt::UserRole)==ROLE_COLUMN) {
			// TODO make these customizable
            menu.addAction(u"SELECT %1 FROM %2"_s.arg(item->text(0), table));
            menu.addAction(u"SELECT DISTINCT %1 FROM %2"_s.arg(item->text(0), table));
		}

        #if 0 // TODO
		if(m_canshowcreate) {
			menu.addSeparator();
			showcreate = menu.addAction(tr("Show create script"));
			showcreate->setProperty("tablename", table);
		}
#endif

	}

	if(!menu.isEmpty())
		menu.addSeparator();
    QAction *refreshAct = menu.addAction(i18n("Refresh"));

	QAction *a = menu.exec(m_view->mapToGlobal(point));
    if(a) {
		if(a==refreshAct)
            Q_EMIT refreshRequested();
        //else if(a==showcreate)
//			m_connection->getCreateScript(a->property("tablename").toString());
		else
            Q_EMIT queryRequested(a->text());
	}
}
