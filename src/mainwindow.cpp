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
#include <KApplication>
#include <KActionCollection>
#include <KAction>
#include <KStandardAction>
#include <KFileDialog>
#include <KMessageBox>
#include <KStandardGuiItem>

#include <QTabWidget>
#include <QTreeView>

#include "mainwindow.h"
#include "connectiondialog.h"
#include "db/connection.h"

#include "querywidget.h"
#include "scriptwidget.h"
#include "tablelistwidget.h"

MainWindow::MainWindow(Connection *connection, QWidget *parent)
	: KXmlGuiWindow(parent), m_connection(connection), m_querytabs(0)
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowIcon(KIcon("kquerybrowser"));
	setCaption(connection->name());
	setupActions();

	// Create tabs for query and script widgets. This is the central widget
	m_tabs = new QTabWidget();
	setCentralWidget(m_tabs);
	m_tabs->setMovable(true);
	connect(m_tabs, SIGNAL(tabCloseRequested(int)), this, SLOT(closeTab(int)));
	connect(m_tabs, SIGNAL(currentChanged(int)), this, SLOT(currentTabChanged(int)));

	newQueryTab();

	// Create the table list dock widget
	TableListWidget *tablelist = new TableListWidget(this);
	tablelist->setObjectName("tablelist");
	tablelist->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, tablelist);
	connect(m_connection, SIGNAL(dbStructure(Database)), tablelist, SLOT(refreshTree(Database)));
	connect(tablelist, SIGNAL(runQuery(QString)), this, SLOT(runQuery(QString)));
	QAction *showtables = actionCollection()->action("showtables");
	connect(tablelist, SIGNAL(visibilityChanged(bool)), showtables, SLOT(setChecked(bool)));
	connect(showtables, SIGNAL(triggered(bool)), tablelist, SLOT(setVisible(bool)));
	connect(tablelist, SIGNAL(refresh()), m_connection, SIGNAL(needDbStructure()));
	m_connection->getDbStructure();

	// Set up XML GUI
	setupGUI(Default, "kquerybrowserui.rc");
}

MainWindow::~MainWindow()
{
	delete m_connection;
}

void MainWindow::setupActions()
{
	KStandardAction::quit(kapp, SLOT(quit()), actionCollection());

	KAction *newQueryTab= new KAction(KIcon("tab-new"), tr("New query"), this);
	actionCollection()->addAction("newquerytab", newQueryTab);
	connect(newQueryTab, SIGNAL(triggered()), this, SLOT(newQueryTab()));

	KAction *newScriptTab = new KAction(KIcon("document-new"), tr("New script"), this);
	actionCollection()->addAction("newscripttab", newScriptTab);
	connect(newScriptTab, SIGNAL(triggered()), this, SLOT(newScriptTab()));

	KAction *openScript = new KAction(KIcon("document-open"), tr("Open script..."), this);
	actionCollection()->addAction("openscript", openScript);
	connect(openScript, SIGNAL(triggered()), this, SLOT(openScript()));

	KAction *saveScript = new KAction(KIcon("document-save"), tr("Save script"), this);
	actionCollection()->addAction("savescript", saveScript);
	connect(saveScript, SIGNAL(triggered()), this, SLOT(saveScript()));
	saveScript->setEnabled(false);

	KAction *saveScriptAs = new KAction(KIcon("document-save-as"), tr("Save script as..."), this);
	actionCollection()->addAction("savescriptas", saveScriptAs);
	connect(saveScriptAs, SIGNAL(triggered()), this, SLOT(saveScriptAs()));
	saveScriptAs->setEnabled(false);

	KAction *showTableDock = new KAction(tr("Show tables"), this);
	showTableDock->setCheckable(true);
	actionCollection()->addAction("showtables", showTableDock);


	KAction *clearResultView = new KAction(tr("Clear"), this);
	actionCollection()->addAction("resultsclear", clearResultView);
	connect(clearResultView, SIGNAL(triggered()), this, SLOT(clearResults()));
}

void MainWindow::currentTabChanged(int index)
{
	ScriptWidget *sw = qobject_cast<ScriptWidget*>(m_tabs->widget(index));

	actionCollection()->action("savescript")->setEnabled(sw!=0);
	actionCollection()->action("savescriptas")->setEnabled(sw!=0);
}

void MainWindow::newTab(QWidget *widget, const QString& title)
{
	m_connection->connectContext(widget);
	m_tabs->addTab(widget, title);

	if(qobject_cast<ScriptWidget*>(widget))
		connect(widget, SIGNAL(nameChange(QString)), this, SLOT(tabNameChange(QString)));

	if(m_tabs->count()>1)
		m_tabs->setTabsClosable(true);
	m_tabs->setCurrentWidget(widget);
}

void MainWindow::newQueryTab()
{
	newTab(new QueryWidget(this), tr("Query %1").arg(++m_querytabs));
}

void MainWindow::newScriptTab()
{
	ScriptWidget *sw = new ScriptWidget(KUrl(), this);
	if(sw->isValid())
		newTab(sw, sw->documentName());
	else
		delete sw;
}

void MainWindow::openScript()
{
	KUrl url = KFileDialog::getOpenUrl(KUrl(), "*.sql|SQL scripts\n*.txt|Text files\n*|All files", this);

	if(!url.isEmpty()) {
		ScriptWidget *sw = new ScriptWidget(url, this);
		if(sw->isValid())
			newTab(sw, sw->documentName());
		else
			delete sw;
	}
}

void MainWindow::closeTab(int index)
{
	QWidget *widget = m_tabs->widget(index);
	ScriptWidget *sw = qobject_cast<ScriptWidget*>(widget);
	if(sw!=0 && sw->isUnsaved()) {
		int act = KMessageBox::warningYesNoCancel(
					this,
					tr("The script \"%1\" has been modified.\nDo you want to save your changes or discard them?").arg(sw->documentName()),
					tr("Close script"),
					KStandardGuiItem::save(),
					KStandardGuiItem::discard()
					);
		if(act==KStandardGuiItem::Yes) {
			if(sw->documentUrl().isEmpty()) {
				KUrl url = KFileDialog::getSaveUrl(sw->documentUrl(), "*.sql|SQL scripts\n*.txt|Text files\n*|All files", this);
				if(!url.isEmpty())
					sw->saveAs(url);
				else
					return;
			} else
				sw->save();
		} else if(act==KStandardGuiItem::Cancel) {
			return;
		}
	}

	delete widget;
	if(m_tabs->count()==1)
		m_tabs->setTabsClosable(false);
}

void MainWindow::clearResults()
{
	QMetaObject::invokeMethod(m_tabs->currentWidget(), "clearResults", Qt::DirectConnection);
}

void MainWindow::saveScript()
{
	ScriptWidget *sw = qobject_cast<ScriptWidget*>(m_tabs->currentWidget());
	if(sw!=0) {
		if(sw->documentUrl().isEmpty())
			saveScriptAs();
		else
			sw->save();
	}
}

void MainWindow::saveScriptAs()
{
	ScriptWidget *sw = qobject_cast<ScriptWidget*>(m_tabs->currentWidget());
	if(sw!=0) {
		KUrl url = KFileDialog::getSaveUrl(sw->documentUrl(), "*.sql|SQL scripts\n*.txt|Text files\n*|All files", this);
		if(!url.isEmpty())
			sw->saveAs(url);
	}
}

void MainWindow::tabNameChange(const QString &name)
{
	ScriptWidget *tab = qobject_cast<ScriptWidget*>(sender());
	int i = m_tabs->indexOf(tab);
	if(i>=0) {
		m_tabs->setTabText(i, name);
	} else {
		qWarning("tabNameChange was passed a widget that is not a tab!");
	}
}

void MainWindow::runQuery(const QString &query)
{
	if(query.isEmpty())
		return;

	QueryWidget *qw = qobject_cast<QueryWidget*>(m_tabs->currentWidget());
	if(qw!=0) {
		qw->runQuery(query);
	} else {
		newQueryTab();
		qobject_cast<QueryWidget*>(m_tabs->currentWidget())->runQuery(query);
	}
}
