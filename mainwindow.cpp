#include <KApplication>
#include <KActionCollection>
#include <KAction>
#include <KStandardAction>

#include <QTabWidget>
#include <QTreeView>

#include "mainwindow.h"
#include "connectiondialog.h"
#include "connection.h"

#include "querywidget.h"
#include "scriptwidget.h"
#include "tablelistwidget.h"

MainWindow::MainWindow(Connection *connection, QWidget *parent)
	: KXmlGuiWindow(parent), m_connection(connection), m_querytabs(0)
{
	m_connection->setParent(this);
	setupActions();

	// Create tabs for query and script widgets. This is the central widget
	m_tabs = new QTabWidget();
	setCentralWidget(m_tabs);
	m_tabs->setMovable(true);
	connect(m_tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));

	newQueryTab();

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

	KAction *newQueryTab= new KAction(KIcon("document-new"), tr("New query"), this);
	actionCollection()->addAction("newquerytab", newQueryTab);
	connect(newQueryTab, SIGNAL(triggered()), this, SLOT(newQueryTab()));

	KAction *newScriptTab = new KAction(KIcon("document-new"), tr("New script"), this);
	actionCollection()->addAction("newscripttab", newScriptTab);
	connect(newScriptTab, SIGNAL(triggered()), this, SLOT(newScriptTab()));

	KAction *openScript = new KAction(KIcon("document-open"), tr("Open script..."), this);
	actionCollection()->addAction("openscript", openScript);

	KAction *saveScript = new KAction(KIcon("document-save"), tr("Save script"), this);
	actionCollection()->addAction("savescript", saveScript);

	KAction *saveScriptAs = new KAction(KIcon("document-save-as"), tr("Save script as..."), this);
	actionCollection()->addAction("savescriptas", saveScriptAs);

}

void MainWindow::newQueryTab()
{
	QueryWidget *qw = new QueryWidget(this);
	m_connection->connectContext(qw);
	m_tabs->addTab(qw, tr("Query %1").arg(++m_querytabs));
	if(m_tabs->count()>1)
		m_tabs->setTabsClosable(true);
	m_tabs->setCurrentWidget(qw);
}

void MainWindow::newScriptTab()
{
	ScriptWidget *sw = new ScriptWidget(this);
	m_connection->connectContext(sw);
	m_tabs->addTab(sw, tr("New script"));
	if(m_tabs->count()>1)
		m_tabs->setTabsClosable(true);
	m_tabs->setCurrentWidget(sw);
}

void MainWindow::closeTab(int index)
{
	QWidget *widget = m_tabs->widget(index);
	delete widget;
	if(m_tabs->count()==1)
		m_tabs->setTabsClosable(false);
}

