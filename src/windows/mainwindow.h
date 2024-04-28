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
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>
#include <KBookmarkOwner>
#include <QJsonDocument>

class QTabWidget;
class QWebEngineProfile;
class KRecentFilesAction;

class DatabaseAdapter;
class WebApi;
class TableListDock;
class DbListDock;

class MainWindow : public KXmlGuiWindow, public KBookmarkOwner
{
	Q_OBJECT
public:
    MainWindow(DatabaseAdapter *database, QWidget *parent=nullptr);
	~MainWindow();

    QUrl currentUrl() const override;
    QString currentTitle() const override;
    void openBookmark(const KBookmark &bm, Qt::MouseButtons mb, Qt::KeyboardModifiers km) override;

private Q_SLOTS:
    void newConnection();
    void newQueryTab();
    void newScriptTab();
    void openScriptTab(const QUrl &url);
    void openScriptDialog();
    void saveCurrentScript();
    void saveCurrentScriptAs();

    void runInQueryTab(const QString &query);
    void findInCurrentTab();
    void findNextInCurrentTab();
    void findPrevInCurrentTab();
    void clearResultsInCurrentTab();
    void runCurrentScript();
    void runCurrentScriptSelection();
    void currentTabChanged(int idx);
    void closeTab(int index);

private:
    Q_INVOKABLE QJsonDocument showBigValue(const QJsonObject &kwargs);

	void setupActions();
	void readSettings();
	void writeSettings();

	QTabWidget *m_tabs;
    KRecentFilesAction *m_recent;

	QWebEngineProfile *m_wep;
    DatabaseAdapter *m_database;
    WebApi *m_webapi;

    TableListDock *m_tableListDock;
    DbListDock *m_dbListDock;
};

#endif
