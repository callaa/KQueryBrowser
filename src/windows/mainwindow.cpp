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
#include "windows/mainwindow.h"
#include "windows/newconnectiondialog.h"
#include "windows/bigvaluedialog.h"
#include "queryview/webengine.h"
#include "queryview/queryview.h"
#include "queryview/webapi.h"
#include "widgets/tablelist.h"
#include "widgets/dblist.h"
#include "widgets/scripteditor.h"
#include "db/database.h"
#include "utils/bookmarks.h"

#include <QDebug>
#include <QApplication>
#include <QTabWidget>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>
#include <KBookmarkMenu>
#include <KStandardAction>
#include <KActionCollection>
#include <KRecentFilesAction>
#include <KActionMenu>
#include <KLocalizedString>
#include <KSharedConfig>
#include <KConfigGroup>

using namespace Qt::StringLiterals;

MainWindow::MainWindow(DatabaseAdapter *database, QWidget *parent)
    : KXmlGuiWindow(parent), m_database(database)
{
    Q_ASSERT(m_database);
    m_database->setParent(this);

	setAttribute(Qt::WA_DeleteOnClose, true);
    setWindowIcon(QIcon::fromTheme(u"kquerybrowser"_s));

	// Create the shared web engine profile used by all the query views
    m_webapi = new WebApi(this);
    m_webapi->registerMethod(QStringLiteral("showBigValue"), this, "showBigValue(QJsonObject)");
    m_wep = initWebEngineProfile(m_webapi, this);

	// Create tabs for query and script widgets. This is the central widget
	m_tabs = new QTabWidget();
    m_tabs->setMovable(true);
    m_tabs->setDocumentMode(true);
	setCentralWidget(m_tabs);

    connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);
    connect(m_tabs, &QTabWidget::currentChanged, this, &MainWindow::currentTabChanged);

	// Create the table list dock widget
    m_tableListDock = new TableListDock(this);
    m_tableListDock->setObjectName("tablelist");
    m_tableListDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_tableListDock);

    connect(m_database, &DatabaseAdapter::schemaRefreshed, m_tableListDock, &TableListDock::setSchema);
    connect(m_tableListDock, &TableListDock::queryRequested, this, &MainWindow::runInQueryTab);
    connect(m_tableListDock, &TableListDock::refreshRequested, m_database, &DatabaseAdapter::refreshSchema);

	// Create the database list dock widget
    m_dbListDock = new DbListDock(this);
    m_dbListDock->setObjectName("databaselist");
    m_dbListDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    addDockWidget(Qt::RightDockWidgetArea, m_dbListDock);
    connect(m_database, &DatabaseAdapter::otherDatabases, m_dbListDock, &DbListDock::setDatabaseList);
    connect(m_dbListDock, &DbListDock::refreshRequested, m_database, &DatabaseAdapter::refreshOtherDatabases);
    connect(m_dbListDock, &DbListDock::newConnection, this, [this](const QString &db) {
        QUrl url = m_database->url();
        url.setPath(u'/' + db);
        NewConnectionDialog::openNewDialog(url.toString());
    });

    // Set up XML GUI
    setupActions();
    readSettings();
    setupGUI(Default, u":kquerybrowserui.rc"_s);

    QMenu *bmmenu = findChild<QMenu*>("bookmarks");
    if(!bmmenu) {
        qWarning("Bookmark menu not present!");
    } else {
        new KBookmarkMenu(
            sharedBookmarkManager(),
            this,
            bmmenu);
    }

    m_database->refreshSchema();
    m_database->refreshOtherDatabases();
    newQueryTab();
}

MainWindow::~MainWindow()
{
	writeSettings();

    // Explicitly delete tabs to ensure all webviews are deleted before the shared profile.
    while(m_tabs->count() > 0) {
        delete m_tabs->widget(0);
    }
}

void MainWindow::setupActions()
{
    auto ac = actionCollection();
	// File menu actions
    KStandardAction::quit(qApp, &QApplication::quit, ac);

    m_recent = KStandardAction::openRecent(this, &MainWindow::openScriptTab, this);
    ac->addAction(u"openrecentscript"_s, m_recent);

    QAction *newQueryTab = new QAction(QIcon::fromTheme(u"tab-new"_s), i18n("New Query"), this);
    ac->addAction(u"newquerytab"_s, newQueryTab);
	connect(newQueryTab, &QAction::triggered, this, &MainWindow::newQueryTab);

    QAction *newScriptTab = new QAction(QIcon::fromTheme(u"document-new"_s), i18n("New Script"), this);
    ac->addAction(u"newscripttab"_s, newScriptTab);
    connect(newScriptTab, &QAction::triggered, this, &MainWindow::newScriptTab);

    QAction *openScript = new QAction(QIcon::fromTheme(u"document-open"_s), i18n("Open Script..."), this);
    ac->addAction(u"openscript"_s, openScript);
	connect(openScript, &QAction::triggered, this, &MainWindow::openScriptDialog);

    QAction *saveScript = new QAction(QIcon::fromTheme(u"document-save"_s), i18n("Save Script"), this);
    ac->addAction(u"savescript"_s, saveScript);
    connect(saveScript, &QAction::triggered, this, &MainWindow::saveCurrentScript);

    QAction *saveScriptAs = new QAction(QIcon::fromTheme(u"document-save-as"_s), i18n("Save Script As..."), this);
    ac->addAction(u"savescriptas"_s, saveScriptAs);
    connect(saveScriptAs, &QAction::triggered, this, &MainWindow::saveCurrentScriptAs);

	// Edit menu actions
    KStandardAction::find(this, &MainWindow::findInCurrentTab, ac);
    KStandardAction::findNext(this, &MainWindow::findNextInCurrentTab, ac);
    KStandardAction::findPrev(this, &MainWindow::findPrevInCurrentTab, ac);

    QAction *runScript = new QAction(QIcon::fromTheme(u"run-build"_s), i18n("Run script"), this);
    ac->addAction(u"runscript"_s, runScript);
    connect(runScript, &QAction::triggered, this, &MainWindow::runCurrentScript);

    QAction *runScriptSelection = new QAction(QIcon::fromTheme(u"debug-run-cursor"_s), i18n("Run selection"), this);
    ac->addAction(u"runscriptselection"_s, runScriptSelection);
    connect(runScriptSelection, &QAction::triggered, this, &MainWindow::runCurrentScriptSelection);

    QAction *clearResultView = new QAction(QIcon::fromTheme(u"edit-clear-history"_s), i18n("Clear results"), this);
    ac->addAction(u"resultsclear"_s, clearResultView);
    connect(clearResultView, &QAction::triggered, this, &MainWindow::clearResultsInCurrentTab);

	// Settings menu actions
    QAction *showTableDock = new QAction(i18n("Show Tables"), this);
	showTableDock->setCheckable(true);
    ac->addAction(u"showtables"_s, showTableDock);
    connect(m_tableListDock, &TableListDock::visibilityChanged, showTableDock, &QAction::setChecked);
    connect(showTableDock, &QAction::triggered, m_tableListDock, &TableListDock::setVisible);

    QAction *showDatabaseDock = new QAction(i18n("Show Databases"), this);
	showDatabaseDock->setCheckable(true);
    ac->addAction(u"showdatabases"_s, showDatabaseDock);
    connect(m_dbListDock, &DbListDock::visibilityChanged, showDatabaseDock, &QAction::setChecked);
    connect(showDatabaseDock, &QAction::triggered, m_dbListDock, &DbListDock::setVisible);

	// Other actions
    QAction *newConnection = new QAction(i18n("New Connection"), this);
    ac->addAction(u"newconnection"_s, newConnection);
	connect(newConnection, &QAction::triggered, this, &MainWindow::newConnection);
}

static const QString RECENT_FILES_CONFIG_GROUP = u"Recent Files"_s;

void MainWindow::readSettings()
{
    m_recent->loadEntries(KSharedConfig::openConfig()->group(RECENT_FILES_CONFIG_GROUP));
}

void MainWindow::writeSettings()
{
	KSharedConfigPtr cfg = KSharedConfig::openConfig();
    m_recent->saveEntries(cfg->group(RECENT_FILES_CONFIG_GROUP));
	cfg->sync();
}

QUrl MainWindow::currentUrl() const
{
    // TODO we should save the password in a wallet rather than include it in the URL
    return m_database->url();
}

QString MainWindow::currentTitle() const
{
    // TODO
    QUrl url = m_database->url();
    return url.path().mid(1);
}

void MainWindow::openBookmark(const KBookmark &bm, Qt::MouseButtons mb, Qt::KeyboardModifiers km)
{
    Q_UNUSED(mb);
    Q_UNUSED(km);

    NewConnectionDialog::openNewDialog(bm.url().toString());
}

QJsonDocument MainWindow::showBigValue(const QJsonObject &kwargs) {
    auto dlg = new BigValueDialog(this);
    dlg->setAttribute(Qt::WA_DeleteOnClose);


    const QString format = kwargs[u"format"].toString();

    if(format == u"b64"_s) {
        dlg->setBinaryValue(QByteArray::fromBase64(kwargs[u"data"].toString().toLatin1()));

    } else {
        dlg->setTextValue(kwargs[u"data"].toString());
    }

    dlg->show();

    return QJsonDocument(QJsonObject());
}

/**
 * Open the new connection dialog
 */
void MainWindow::newConnection()
{
    NewConnectionDialog::openNewDialog();
}

/**
 * Open a fresh new query tab
 */
void MainWindow::newQueryTab()
{
    const auto idx = m_tabs->addTab(new QueryView(m_database, m_webapi, m_wep, true), i18n("Query %1", m_tabs->count() + 1));
    m_tabs->setCurrentIndex(idx);
    m_tabs->setTabsClosable(m_tabs->count() > 1);
}

/**
 * Open a fresh new script tab
 */
void MainWindow::newScriptTab()
{
    openScriptTab(QUrl());
}

/**
 * Open a script and load it into a new tab
 * @param url
 */
void MainWindow::openScriptTab(const QUrl &url)
{
    auto *editor = new ScriptEditor(url, m_database, m_webapi, m_wep);
    const auto idx = m_tabs->addTab(editor, editor->documentName());

    connect(editor, &ScriptEditor::modifiedChanged, editor, [this, editor]() {
        for(int i=0;i<m_tabs->count();++i) {
            if(m_tabs->widget(i) == editor) {
                m_tabs->setTabText(i, editor->documentName());
                break;
            }
        }
    });

    // Note: we can connect this directly because the selection state can change
    // only when the tab is active
    auto runSelectionAction = findChild<QAction*>("runscriptselection");
    Q_ASSERT(runSelectionAction);
    connect(editor, &ScriptEditor::runnableSelectionChanged, runSelectionAction, &QAction::setEnabled);

    m_tabs->setCurrentIndex(idx);
    m_tabs->setTabsClosable(m_tabs->count() > 1);
}

/**
 * Show the open file dialog for choosing a script.
 * This will eventually call openScriptTab()
 */
void MainWindow::openScriptDialog()
{
    const QUrl url = QFileDialog::getOpenFileUrl(this, QString(), QUrl(), u"SQL scripts (*.sql);;Text files (*.txt);;All files (*)"_s);
    if(!url.isEmpty()) {
        openScriptTab(url);
        m_recent->addUrl(url);
    }
}

/**
 * If the currently open tab is a script, save it.
 * "Save as" dialog is shown if the script has no filename yet.
 */
void MainWindow::saveCurrentScript()
{
    auto scriptTab = qobject_cast<ScriptEditor*>(m_tabs->currentWidget());
    if(scriptTab) {
        if(scriptTab->documentUrl().isEmpty())
            saveCurrentScriptAs();
        else
            scriptTab->save();
    }
}

/**
 * Show a "save as" dialog and save the current script.
 */
void MainWindow::saveCurrentScriptAs()
{
    auto scriptTab = qobject_cast<ScriptEditor*>(m_tabs->currentWidget());
    if(scriptTab) {
        const QUrl url = QFileDialog::getSaveFileUrl(this, QString(), scriptTab->documentUrl(), u"SQL scripts (*.sql);;Text files (*.txt);;All files (*)"_s);
        if(!url.isEmpty()) {
            if(scriptTab->saveAs(url))
                m_recent->addUrl(url);
        }
    }
}

/**
 * Run a query in a query tab.
 * If the current tab is not a query tab, open a new query tab.
 *
 * @param query
 */
void MainWindow::runInQueryTab(const QString &query)
{
    auto queryTab = qobject_cast<QueryView*>(m_tabs->currentWidget());
    if(!queryTab) {
        newQueryTab();
        queryTab = qobject_cast<QueryView*>(m_tabs->currentWidget());
        Q_ASSERT(queryTab);
    }

    queryTab->makeQuery(query);
}

/**
 * Execute the "Find" function in the currently active tab
 */
void MainWindow::findInCurrentTab()
{
    auto current = m_tabs->currentWidget();
    auto queryTab = qobject_cast<QueryView*>(current);
    if(queryTab) {
        queryTab->find();
    }
}

/**
 * Execute the "Find next" function in the currently active tab
 */
void MainWindow::findNextInCurrentTab()
{
    auto current = m_tabs->currentWidget();
    auto queryTab = qobject_cast<QueryView*>(current);
    if(queryTab) {
        queryTab->findNext();
    }
}

/**
 * Execute the "Find previous" function in the currently active tab
 */
void MainWindow::findPrevInCurrentTab()
{
    auto current = m_tabs->currentWidget();
    auto queryTab = qobject_cast<QueryView*>(current);
    if(queryTab) {
        queryTab->findPrev();
    }
}

/**
 * Clear the query results in the currently active tab
 */
void MainWindow::clearResultsInCurrentTab()
{
    auto current = m_tabs->currentWidget();
    auto queryTab = qobject_cast<QueryView*>(current);
    if(queryTab) {
        queryTab->clearResults();
        return;
    }
    auto scriptTab = qobject_cast<ScriptEditor*>(current);
    if(scriptTab) {
        scriptTab->clearResults();
    }
}

void MainWindow::runCurrentScript()
{
    auto scriptTab = qobject_cast<ScriptEditor*>(m_tabs->currentWidget());
    if(scriptTab) {
        scriptTab->runScript();
    }
}

void MainWindow::runCurrentScriptSelection()
{
    auto scriptTab = qobject_cast<ScriptEditor*>(m_tabs->currentWidget());
    if(scriptTab) {
        scriptTab->runSelectedScript();
    }
}

void MainWindow::closeTab(int index)
{
    auto tab = m_tabs->widget(index);
    auto scriptTab = qobject_cast<ScriptEditor*>(tab);
    if(scriptTab && scriptTab->isUnsaved()) {
        int act = QMessageBox::warning(
            this,
            i18n("Close script"),
            i18n("The script \"%1\" has been modified.\nDo you want to save your changes or discard them?", scriptTab->documentName()),
            QMessageBox::StandardButton::Save | QMessageBox::StandardButton::Discard | QMessageBox::StandardButton::Cancel
            );
        if(act==QMessageBox::StandardButton::Save) {
            m_tabs->setCurrentIndex(index);
            saveCurrentScript();
        } else if(act==QMessageBox::Cancel) {
            return;
        }
    }
    delete tab;
    m_tabs->setTabsClosable(m_tabs->count() > 1);
}

void MainWindow::currentTabChanged(int index)
{
    if(qobject_cast<ScriptEditor*>(m_tabs->widget(index)))
        stateChanged(u"tab-script"_s);
    else
        stateChanged(u"tab-query"_s);
}
