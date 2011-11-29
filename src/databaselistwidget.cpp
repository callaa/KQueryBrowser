#include <QListWidget>
#include <QMenu>
#include <QFont>

#include "databaselistwidget.h"

DatabaseListWidget::DatabaseListWidget(const QString& current, QWidget *parent) :
	QDockWidget(tr("Databases"), parent), m_current(current)
{
	m_view = new QListWidget(this);
	m_view->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

	setWidget(m_view);
}

void DatabaseListWidget::refreshList(const QStringList &databases)
{
	m_view->clear();
	foreach(const QString& db, databases) {
		QListWidgetItem *item = new QListWidgetItem(db, m_view);
		if(db == m_current) {
			QFont font = item->font();
			font.setBold(true);
			item->setFont(font);
		}
		m_view->addItem(item);
	}
}

void DatabaseListWidget::customContextMenu(const QPoint& point)
{
	QMenu menu;
	QAction *refreshAct = menu.addAction("Refresh");

	QAction *a = menu.exec(m_view->mapToGlobal(point));
	if(a!=0) {
		emit refresh();
	}
}
