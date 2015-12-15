
#include "databaselistwidget.h"

#include <QMenu>
#include <QFont>
#include <QListWidget>

DatabaseListWidget::DatabaseListWidget(bool canswitch, QWidget *parent) :
	QDockWidget(tr("Databases"), parent), m_canswitch(canswitch)
{
	m_view = new QListWidget(this);
	m_view->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(m_view, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenu(QPoint)));

	setWidget(m_view);
}

void DatabaseListWidget::refreshList(const QStringList &databases, const QString& current)
{
	m_view->clear();
	foreach(const QString& db, databases) {
		QListWidgetItem *item = new QListWidgetItem(db, m_view);
		if(db == current) {
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

	QListWidgetItem *item = m_view->itemAt(point);
	QAction *newConnect=0, *switchDb=0;
	if(item) {
		newConnect = menu.addAction(tr("New connection"));
		if(m_canswitch)
			switchDb = menu.addAction(tr("Switch database"));
	}

	if(!menu.isEmpty())
		menu.addSeparator();

	QAction *refreshAct = menu.addAction(tr("Refresh"));

	QAction *a = menu.exec(m_view->mapToGlobal(point));
	if(a!=0) {
		if(a==refreshAct)
			emit refresh();
		else if(a==newConnect)
			emit newConnection(item->text());
		else if(a==switchDb)
			emit switchDatabase(item->text());
	}
}
