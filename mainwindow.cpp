#include <KApplication>
#include <KActionCollection>
#include <KStandardAction>

#include <QTabWidget>
#include <QTreeView>

#include "mainwindow.h"
#include "connectiondialog.h"
#include "connection.h"

#include "querywidget.h"
#include "tablelistwidget.h"

MainWindow::MainWindow(Connection *connection, QWidget *parent)
	: KXmlGuiWindow(parent), m_connection(connection)
{
	m_connection->setParent(this);
	setupActions();

	// Create tabs for query and script widgets. This is the central widget
	QTabWidget *tabs = new QTabWidget();
	setCentralWidget(tabs);

	QueryWidget *qw = new QueryWidget(this);
	m_connection->connectContext(qw);
	tabs->addTab(qw, tr("Query"));

	// Create the table list dock widget
	TableListWidget *tablelist = new TableListWidget(this);
	tablelist->setObjectName("tablelist");
	tablelist->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	tablelist->setFeatures(QDockWidget::DockWidgetFloatable|QDockWidget::DockWidgetMovable);
	addDockWidget(Qt::RightDockWidgetArea, tablelist);
	connect(m_connection, SIGNAL(dbStructure(Database)), tablelist, SLOT(refreshTree(Database)));
	m_connection->getDbStructure();

	// Set up XML GUI
	setupGUI(Default, "kquerybrowserui.rc");
}

void MainWindow::setupActions()
{
	KStandardAction::quit(kapp, SLOT(quit()), actionCollection());
}
