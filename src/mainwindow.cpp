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

#include <QDebug>
#include <QInputDialog>
#include <QSaveFile>
#include <QMenu>
#include <QTabWidget>
#include <QApplication>
#include <QFileDialog>
#include <QScopedPointer>

#include <KSharedConfig>
#include <KRecentFilesAction>
#include <KStandardAction>
#include <KActionCollection>
#include <KActionMenu>
#include <KEncodingFileDialog>
#include <KMessageBox>
#include <KStandardGuiItem>
#include <KBookmarkMenu>
#include <KConfigGroup>

MainWindow::MainWindow(db::Connection *connection, QWidget *parent)
	: KXmlGuiWindow(parent), m_connection(connection), m_querytabs(0)
{
	setAttribute(Qt::WA_DeleteOnClose, true);
	setWindowIcon(QIcon::fromTheme("kquerybrowser"));
	nameChange(connection->name());
	setupActions();

	connect(connection, &db::Connection::nameChanged, this, &MainWindow::nameChange);
	connect(connection, &db::Connection::newScript, this, &MainWindow::newScriptTab);

	// Create tabs for query and script widgets. This is the central widget
	m_tabs = new QTabWidget();
	setCentralWidget(m_tabs);
	m_tabs->setMovable(true);
	connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
	connect(m_tabs, &QTabWidget::currentChanged, this, &MainWindow::currentTabChanged);
	connect(m_tabs, &QTabWidget::tabBarDoubleClicked, this, &MainWindow::renameQueryTab);

	newQueryTab();

	// Create the table list dock widget
	TableListWidget *tablelist = new TableListWidget(m_connection->isCapable(db::Connection::SHOW_CREATE), this);
	tablelist->setObjectName("tablelist");
	tablelist->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, tablelist);
	connect(m_connection, &db::Connection::dbStructure, tablelist, &TableListWidget::refreshTree);
	connect(tablelist, &TableListWidget::runQuery, this, &MainWindow::runQuery);
	connect(tablelist, &TableListWidget::showCreate, m_connection, &db::Connection::needCreateTable);
	m_connection->getDbStructure();

	QAction *showtables = actionCollection()->action("showtables");
	connect(tablelist, SIGNAL(visibilityChanged(bool)), showtables, SLOT(setChecked(bool)));
	connect(showtables, SIGNAL(triggered(bool)), tablelist, SLOT(setVisible(bool)));
	connect(tablelist, SIGNAL(refresh()), m_connection, SIGNAL(needDbStructure()));

	// Create the database list dock widget
	DatabaseListWidget *dblist = new DatabaseListWidget(m_connection->isCapable(db::Connection::SWITCH_DB), this);
	dblist->setObjectName("databaselist");
	dblist->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
	addDockWidget(Qt::RightDockWidgetArea, dblist);
	connect(m_connection, &db::Connection::dbList, dblist, &DatabaseListWidget::refreshList);
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
	setupGUI(Default, ":ui/kquerybrowserui.rc");

	QMenu *bmmenu = findChild<QMenu*>("bookmarks");
	if(!bmmenu) {
		qWarning("Bookmark menu not present!");
	} else {
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
	KStandardAction::quit(qApp, SLOT(quit()), actionCollection());
	m_recent = KStandardAction::openRecent(this, SLOT(openScript(QUrl)), 0);
	actionCollection()->addAction("openrecentscript", m_recent);

	QAction *newQueryTab= new QAction(QIcon::fromTheme("tab-new"), tr("New Query"), this);
	actionCollection()->addAction("newquerytab", newQueryTab);
	connect(newQueryTab, &QAction::triggered, this, &MainWindow::newQueryTab);

	QAction *newScriptTab = new QAction(QIcon::fromTheme("document-new"), tr("New Script"), this);
	actionCollection()->addAction("newscripttab", newScriptTab);
	connect(newScriptTab, &QAction::triggered, this, &MainWindow::newBlankScriptTab);

	QAction *openScript = new QAction(QIcon::fromTheme("document-open"), tr("Open Script..."), this);
	actionCollection()->addAction("openscript", openScript);
	connect(openScript, &QAction::triggered, this, &MainWindow::openScriptDialog);

	QAction *saveScript = new QAction(QIcon::fromTheme("document-save"), tr("Save Script"), this);
	actionCollection()->addAction("savescript", saveScript);
	connect(saveScript, &QAction::triggered, this, &MainWindow::saveScript);
	saveScript->setEnabled(false);

	QAction *saveScriptAs = new QAction(QIcon::fromTheme("document-save-as"), tr("Save Script As..."), this);
	actionCollection()->addAction("savescriptas", saveScriptAs);
	connect(saveScriptAs, &QAction::triggered, this, &MainWindow::saveScriptAs);
	saveScriptAs->setEnabled(false);

	// Export submenu
	KActionMenu *exportmenu = new KActionMenu(QIcon::fromTheme("document-export-table"), tr("Export results"), this);
	actionCollection()->addAction("resultexportmenu", exportmenu);
	QActionGroup *exportgroup = Exporters::instance().multiTableActions(this);
	for(QAction *a : exportgroup->actions())
		exportmenu->addAction(a);
	connect(exportgroup, SIGNAL(triggered(QAction*)),
			this, SLOT(exportResults(QAction*)));


	// Edit menu actions
	KStandardAction::find(this, SLOT(search()), actionCollection());
	KStandardAction::findNext(this, SLOT(findNext()), actionCollection());
	KStandardAction::findPrev(this, SLOT(findPrev()), actionCollection());

	QAction *clearResultView = new QAction(tr("Clear results"), this);
	actionCollection()->addAction("resultsclear", clearResultView);
	connect(clearResultView, &QAction::triggered, this, &MainWindow::clearResults);

	// Settings menu actions
	QAction *showTableDock = new QAction(tr("Show Tables"), this);
	showTableDock->setCheckable(true);
	actionCollection()->addAction("showtables", showTableDock);

	QAction *showDatabaseDock = new QAction(tr("Show Databases"), this);
	showDatabaseDock->setCheckable(true);
	actionCollection()->addAction("showdatabases", showDatabaseDock);

	// Other actions
	QAction *newConnection = new QAction(tr("New Connection"), this);
	actionCollection()->addAction("newconnection", newConnection);
	connect(newConnection, &QAction::triggered, this, &MainWindow::newConnection);
}

void MainWindow::readSettings()
{
	m_recent->loadEntries(KSharedConfig::openConfig()->group("Recent Files"));
}

void MainWindow::writeSettings()
{
	KSharedConfigPtr cfg = KSharedConfig::openConfig();
	m_recent->saveEntries(cfg->group("Recent Files"));
	cfg->sync();
}

void MainWindow::newConnection()
{
	ConnectionDialog::openDialog(QUrl());
}

void MainWindow::newDbConnection(const QString &database)
{
	QUrl url = m_connection->url();
	url.setPath("/" + database);
	ConnectionDialog::openDialog(url);
}

QString MainWindow::currentTitle() const
{
	return m_connection->name();
}

QUrl MainWindow::currentUrl() const
{
	// TODO use prettyUrl() and store password in KWallet
	return m_connection->url();
}

KBookmarkDialog *MainWindow::bookmarkDialog(KBookmarkManager *mgr, QWidget *parent)
{
	return new BookmarkDialog(mgr, parent);
}

void MainWindow::openBookmark(const KBookmark& bookmark, Qt::MouseButtons mb, Qt::KeyboardModifiers km)
{
	Q_UNUSED(mb);
	Q_UNUSED(km);
	ConnectionDialog::openDialog(bookmark.url());
}

void MainWindow::currentTabChanged(int index)
{
	if(qobject_cast<ScriptWidget*>(m_tabs->widget(index)))
		stateChanged("tab-script");
	else
		stateChanged("tab-query");
}

void MainWindow::renameQueryTab(int index)
{
	QWidget *widget = m_tabs->widget(index);
	if(qobject_cast<QueryWidget*>(widget)) {
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
	connect(m_connection, &db::Connection::dbStructure, qw, &QueryWidget::dbStructure);
	newTab(qw, tr("Query %1").arg(++m_querytabs));
}

void MainWindow::newScriptTab(const QString& content)
{
	ScriptWidget *sw = new ScriptWidget(QUrl(), this);
	if(sw->isValid()) {
		if(!content.isEmpty())
			sw->setContent(content);
		newTab(sw, sw->documentName());
	} else
		delete sw;
}

void MainWindow::newBlankScriptTab()
{
	newScriptTab(QString());
}

void MainWindow::openScriptDialog()
{
	QUrl url = QFileDialog::getOpenFileUrl(this, QString(), QUrl(), "*.sql|SQL scripts\n*.txt|Text files\n*|All files");

	if(!url.isEmpty())
		if(openScript(url))
			m_recent->addUrl(url);
}

bool MainWindow::openScript(const QUrl& url)
{
	ScriptWidget *sw = new ScriptWidget(url, this);
	if(sw->isValid()) {
		newTab(sw, sw->documentName());
		return true;
	}
	delete sw;
	return false;
}

void MainWindow::closeTab(int index)
{
	QWidget *widget = m_tabs->widget(index);
	ScriptWidget *sw = qobject_cast<ScriptWidget*>(widget);
	if(sw && sw->isUnsaved()) {
		int act = KMessageBox::warningYesNoCancel(
					this,
					tr("The script \"%1\" has been modified.\nDo you want to save your changes or discard them?").arg(sw->documentName()),
					tr("Close script"),
					KStandardGuiItem::save(),
					KStandardGuiItem::discard()
					);
		if(act==KStandardGuiItem::Yes) {
			if(sw->documentUrl().isEmpty()) {
				QUrl url = QFileDialog::getSaveFileUrl(this, QString(), sw->documentUrl(), "*.sql|SQL scripts\n*.txt|Text files\n*|All files");
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
	QMetaObject::invokeMethod(m_tabs->currentWidget(), "clearResults", Qt::AutoConnection);
}

void MainWindow::search()
{
	QMetaObject::invokeMethod(m_tabs->currentWidget(), "showSearch", Qt::AutoConnection);
}

void MainWindow::findNext()
{
	QMetaObject::invokeMethod(m_tabs->currentWidget(), "findNext", Qt::AutoConnection);
}

void MainWindow::findPrev()
{
	QMetaObject::invokeMethod(m_tabs->currentWidget(), "findPrev", Qt::AutoConnection);
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
		QUrl url = QFileDialog::getSaveFileUrl(this, QString(), sw->documentUrl(), "*.sql|SQL scripts\n*.txt|Text files\n*|All files");
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
		QSaveFile file(filename.fileNames.at(0));
		if(!file.open(QSaveFile::WriteOnly)) {
			KMessageBox::error(this, file.errorString());
		} else {
			QScopedPointer<Exporter> exporter(Exporters::instance().get(format));

			exporter->startFile(&file, filename.encoding, m_tabs->tabText(m_tabs->currentIndex()));

			QScopedPointer<TableCellIterator> iterator;
			if(ScriptWidget *sw = qobject_cast<ScriptWidget*>(m_tabs->currentWidget()))
				iterator.reset(sw->tableIterator());
			else if(QueryWidget *qw = qobject_cast<QueryWidget*>(m_tabs->currentWidget()))
				iterator.reset(qw->tableIterator());
			else {
				qFatal("Weird tab encountered!");
				return;
			}

			while(iterator->nextTable())
				exporter->beginTable(iterator.data());

			exporter->done();

			if(!file.commit())
				KMessageBox::error(this, file.errorString());
		}
	}
}

