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
#include <QInputDialog>
#include <KApplication>
#include <KActionCollection>
#include <KRecentFilesAction>
#include <KStandardAction>
#include <KEncodingFileDialog>
#include <KMessageBox>
#include <KSaveFile>
#include <KStandardGuiItem>
#include <KBookmarkMenu>
#include <KMenu>
#include <KTabWidget>

#include "mainwindow.h"
#include "connectiondialog.h"
#include "db/connection.h"

#include "querywidget.h"
#include "scriptwidget.h"
#include "tablelistwidget.h"
#include "databaselistwidget.h"
#include "bookmarks.h"
#include "bookmarkdialog.h"
#include "export/exporter.h"

MainWindow::MainWindow(Connection *connection, QWidget *parent)
	: KXmlGuiWindow(parent), m_connection(connection), m_querytabs(0)
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowIcon(KIcon("kquerybrowser"));
	nameChange(connection->name());
	setupActions();

	connect(connection, SIGNAL(nameChanged(QString)),
			this, SLOT(nameChange(QString)));

	connect(connection, SIGNAL(newScript(QString)),
			this, SLOT(newScriptTab(QString)));

	// Create tabs for query and script widgets. This is the central widget
	m_tabs = new KTabWidget();
	setCentralWidget(m_tabs);
	m_tabs->setMovable(true);
	connect(m_tabs, SIGNAL(closeRequest(QWidget*)),
			this, SLOT(closeTab(QWidget*)));
	connect(m_tabs, SIGNAL(mouseDoubleClick()),
			this, SLOT(newQueryTab()));
	connect(m_tabs, SIGNAL(currentChanged(int)),
			this, SLOT(currentTabChanged(int)));
	connect(m_tabs, SIGNAL(contextMenu(QWidget*, const QPoint&)),
			this, SLOT(tabContextMenu(QWidget*, const QPoint&)));

	newQueryTab();

	// Create the table list dock widget
	TableListWidget *tablelist = new TableListWidget(m_connection->isCapable(Connection::SHOW_CREATE), this);
	tablelist->setObjectName("tablelist");
	tablelist->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, tablelist);
	connect(m_connection, SIGNAL(dbStructure(Database)),
			tablelist, SLOT(refreshTree(Database)));
	connect(tablelist, SIGNAL(runQuery(QString)),
			this, SLOT(runQuery(QString)));
	connect(tablelist, SIGNAL(showCreate(QString)),
			m_connection, SIGNAL(needCreateTable(QString)));
	m_connection->getDbStructure();

	QAction *showtables = actionCollection()->action("showtables");
	connect(tablelist, SIGNAL(visibilityChanged(bool)), showtables, SLOT(setChecked(bool)));
	connect(showtables, SIGNAL(triggered(bool)), tablelist, SLOT(setVisible(bool)));
	connect(tablelist, SIGNAL(refresh()), m_connection, SIGNAL(needDbStructure()));

	// Create the database list dock widget
	DatabaseListWidget *dblist = new DatabaseListWidget(m_connection->isCapable(Connection::SWITCH_DB), this);
	dblist->setObjectName("databaselist");
	dblist->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, dblist);
	connect(m_connection, SIGNAL(dbList(QStringList, QString)),
			dblist, SLOT(refreshList(QStringList, QString)));
	m_connection->getDbList();

	QAction *showdbs = actionCollection()->action("showdatabases");
	connect(dblist, SIGNAL(visibilityChanged(bool)),
			showdbs, SLOT(setChecked(bool)));
	connect(showdbs, SIGNAL(triggered(bool)),
			dblist, SLOT(setVisible(bool)));
	connect(dblist, SIGNAL(refresh()),
			m_connection, SIGNAL(needDbList()));
	connect(dblist, SIGNAL(switchDatabase(QString)),
			m_connection, SIGNAL(switchDatabase(QString)));
	connect(dblist, SIGNAL(newConnection(QString)),
			this, SLOT(newDbConnection(QString)));

	readSettings();

	// Set up XML GUI
	setupGUI(Default, "kquerybrowserui.rc");

	KMenu *bmmenu = findChild<KMenu*>("bookmarks");
	KBookmarkMenu *bmm = new KBookmarkMenu(
			Bookmarks::manager(),
			this,
			bmmenu,
			actionCollection());
	// Note. bookmark menu parent is not set automatically.
	// (This weird behavior can't be fixed without breaking konqueror
	// and possibly some other apps. It will probably be fixed in KDE5)
	bmm->setParent(bmmenu);
}

MainWindow::~MainWindow()
{
	writeSettings();
	delete m_connection;
}

void MainWindow::nameChange(const QString& name)
{
	setCaption(name);
}

void MainWindow::setupActions()
{
	// File menu actions
	KStandardAction::quit(kapp, SLOT(quit()), actionCollection());
	m_recent = KStandardAction::openRecent(this, SLOT(openScript(KUrl)), 0);
	actionCollection()->addAction("openrecentscript", m_recent);

	KAction *newQueryTab= new KAction(KIcon("tab-new"), tr("New Query"), this);
	actionCollection()->addAction("newquerytab", newQueryTab);
	connect(newQueryTab, SIGNAL(triggered()), this, SLOT(newQueryTab()));

	KAction *newScriptTab = new KAction(KIcon("document-new"), tr("New Script"), this);
	actionCollection()->addAction("newscripttab", newScriptTab);
	connect(newScriptTab, SIGNAL(triggered()), this, SLOT(newScriptTab()));

	KAction *openScript = new KAction(KIcon("document-open"), tr("Open Script..."), this);
	actionCollection()->addAction("openscript", openScript);
	connect(openScript, SIGNAL(triggered()), this, SLOT(openScript()));

	KAction *saveScript = new KAction(KIcon("document-save"), tr("Save Script"), this);
	actionCollection()->addAction("savescript", saveScript);
	connect(saveScript, SIGNAL(triggered()), this, SLOT(saveScript()));
	saveScript->setEnabled(false);

	KAction *saveScriptAs = new KAction(KIcon("document-save-as"), tr("Save Script As..."), this);
	actionCollection()->addAction("savescriptas", saveScriptAs);
	connect(saveScriptAs, SIGNAL(triggered()), this, SLOT(saveScriptAs()));
	saveScriptAs->setEnabled(false);

	// Export submenu
	KActionMenu *exportmenu = new KActionMenu(KIcon("document-export-table"), tr("Export results"), this);
	actionCollection()->addAction("resultexportmenu", exportmenu);
	QActionGroup *exportgroup = Exporters::instance().multiTableActions(this);
	foreach(QAction *a, exportgroup->actions())
		exportmenu->addAction(a);
	connect(exportgroup, SIGNAL(triggered(QAction*)),
			this, SLOT(exportResults(QAction*)));


	// Edit menu actions
	KStandardAction::find(this, SLOT(search()), actionCollection());
	KStandardAction::findNext(this, SLOT(findNext()), actionCollection());
	KStandardAction::findPrev(this, SLOT(findPrev()), actionCollection());

	KAction *clearResultView = new KAction(tr("Clear results"), this);
	actionCollection()->addAction("resultsclear", clearResultView);
	connect(clearResultView, SIGNAL(triggered()), this, SLOT(clearResults()));

	// Settings menu actions
	KAction *showTableDock = new KAction(tr("Show Tables"), this);
	showTableDock->setCheckable(true);
	actionCollection()->addAction("showtables", showTableDock);

	KAction *showDatabaseDock = new KAction(tr("Show Databases"), this);
	showDatabaseDock->setCheckable(true);
	actionCollection()->addAction("showdatabases", showDatabaseDock);

	// Other actions
	KAction *newConnection = new KAction(tr("New Connection"), this);
	actionCollection()->addAction("newconnection", newConnection);
	connect(newConnection, SIGNAL(triggered()), this, SLOT(newConnection()));
}

void MainWindow::readSettings()
{
	m_recent->loadEntries(KGlobal::config()->group("Recent Files"));
}

void MainWindow::writeSettings()
{
	m_recent->saveEntries(KGlobal::config()->group("Recent Files"));
	KGlobal::config()->sync();
}

void MainWindow::newConnection(const QUrl& url)
{
	ConnectionDialog::open(url);
}

void MainWindow::newDbConnection(const QString &database)
{
	KUrl url = m_connection->url();
	url.setPath("/" + database);
	ConnectionDialog::open(url);
}

QString MainWindow::currentTitle() const
{
	return m_connection->name();
}

QString MainWindow::currentUrl() const
{
	// TODO use prettyUrl() and store password in KWallet
	return m_connection->url().url();
}

KBookmarkDialog *MainWindow::bookmarkDialog(KBookmarkManager *mgr, QWidget *parent)
{
	return new BookmarkDialog(mgr, parent);
}

void MainWindow::openBookmark(const KBookmark& bookmark, Qt::MouseButtons mb, Qt::KeyboardModifiers km)
{
	Q_UNUSED(mb);
	Q_UNUSED(km);
	newConnection(bookmark.url());
}

void MainWindow::currentTabChanged(int index)
{
	ScriptWidget *sw = qobject_cast<ScriptWidget*>(m_tabs->widget(index));

	if(sw==0)
		stateChanged("tab-query");
	else
		stateChanged("tab-script");
}

void MainWindow::tabContextMenu(QWidget *w, const QPoint &p)
{
	if(qobject_cast<QueryWidget*>(w)) {
		QMenu menu;
		menu.addAction(tr("Rename tab"));
		if(menu.exec(p)) {
			int index = m_tabs->indexOf(w);
			QString name = QInputDialog::getText(this,
					tr("Rename tab"),
					tr("New name"),
					QLineEdit::Normal,
					m_tabs->tabText(index)
					);
			if(!name.isEmpty()) {
				m_tabs->setTabText(index, name);
			}
		}
	}
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
	QueryWidget *qw = new QueryWidget(this);
	connect(m_connection, SIGNAL(dbStructure(const Database&)),
			qw, SIGNAL(dbStructure(const Database&)));
	newTab(qw, tr("Query %1").arg(++m_querytabs));
}

void MainWindow::newScriptTab(const QString& content)
{
	ScriptWidget *sw = new ScriptWidget(KUrl(), this);
	if(sw->isValid()) {
		if(!content.isEmpty())
			sw->setContent(content);
		newTab(sw, sw->documentName());
	} else
		delete sw;
}

void MainWindow::openScript()
{
	KUrl url = KFileDialog::getOpenUrl(KUrl(), "*.sql|SQL scripts\n*.txt|Text files\n*|All files", this);

	if(!url.isEmpty())
		if(openScript(url))
			m_recent->addUrl(url);
}

bool MainWindow::openScript(const KUrl& url)
{
	ScriptWidget *sw = new ScriptWidget(url, this);
	if(sw->isValid()) {
		newTab(sw, sw->documentName());
		return true;
	}
	delete sw;
	return false;
}

void MainWindow::closeTab(QWidget *widget)
{
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

void MainWindow::search()
{
	QMetaObject::invokeMethod(m_tabs->currentWidget(), "showSearch", Qt::DirectConnection);
}

void MainWindow::findNext()
{
	QMetaObject::invokeMethod(m_tabs->currentWidget(), "findNext", Qt::DirectConnection);
}

void MainWindow::findPrev()
{
	QMetaObject::invokeMethod(m_tabs->currentWidget(), "findPrev", Qt::DirectConnection);
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

void MainWindow::exportResults(QAction *action)
{
	QString format = action->objectName().mid(action->objectName().indexOf('_')+1);
	KEncodingFileDialog::Result filename = KEncodingFileDialog::getSaveFileNameAndEncoding(
			"UTF-8",
			QString(),
			"*." + action->property("fileExtension").toString() + "|" + format + "\n*|All files",
			this);

	if(!filename.fileNames.at(0).isEmpty()) {
		KSaveFile file(filename.fileNames.at(0));
		if(!file.open()) {
			KMessageBox::error(this, file.errorString());
		} else {
			Exporter *exporter = Exporters::instance().get(format);

			exporter->startFile(&file, filename.encoding, m_tabs->tabText(m_tabs->currentIndex()));

			TableCellIterator *iterator;
			if(ScriptWidget *sw = qobject_cast<ScriptWidget*>(m_tabs->currentWidget()))
				iterator = sw->tableIterator();
			else if(QueryWidget *qw = qobject_cast<QueryWidget*>(m_tabs->currentWidget()))
				iterator = qw->tableIterator();
			else {
				qFatal("Weird tab encountered!");
				return;
			}

			while(iterator->nextTable())
				exporter->beginTable(iterator);

			delete iterator;
			exporter->done();
			delete exporter;

			if(!file.finalize())
				KMessageBox::error(this, file.errorString());
			file.close();
		}
	}
}

