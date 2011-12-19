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

class Connection;
class KTabWidget;
class KRecentFilesAction;
class KBookmarkDialog;
class KBookmarkManager;

class MainWindow : public KXmlGuiWindow, public KBookmarkOwner
{
	Q_OBJECT
public:
	MainWindow(Connection *connection, QWidget *parent=0);
	~MainWindow();

	void openBookmark(const KBookmark& bookmark, Qt::MouseButtons mb, Qt::KeyboardModifiers km);

	//! Get the title of the current connection
	QString currentTitle() const;

	//! Get the URL of the current connection
	QString currentUrl() const;

	//! Get the bookmark editor dialog
	KBookmarkDialog *bookmarkDialog(KBookmarkManager *mgr, QWidget *parent);

public slots:
	//! Open a new query tab
	void newQueryTab();

	//! Open a new blank script tab
	void newScriptTab(const QString& content=QString());

	//! Open a script file
	void openScript();

	/**
	 * \brief Open the specified script
	 * \param url script file URL
	 * \return true on success
	 */
	bool openScript(const KUrl& url);

	//! Save the contents of the currently open script tab
	void saveScript();

	//! Save the contents of the currently open script tab with a new name
	void saveScriptAs();

	/**
	  \brief Run a query

	  The query is executed in the currently open query tab
	  */
	void runQuery(const QString& query);

	//! Show new connection dialog
	void newConnection(const QUrl& url = QUrl());

protected slots:
	/**
	 * \brief Connection name has changed
	 * \param name the new name
	 */
	void nameChange(const QString& name);

	//! Close a tab
	void closeTab(QWidget *widget);

	//! The "content modified" flag on a tab has changed
	void tabNameChange(const QString& name);

	/**
	 * \brief Open a new database connection on the same server
	 * \param database the name of the database
	 */
	void newDbConnection(const QString &database);

	//! Clear the results view of the current tab
	void clearResults();

	void currentTabChanged(int index);

	void exportResults(QAction *action);
private:
	void setupActions();
	void readSettings();
	void writeSettings();
	void newTab(QWidget *widget, const QString& title);

	KTabWidget *m_tabs;
	Connection *m_connection;
	int m_querytabs;

	KRecentFilesAction *m_recent;
};

#endif // MAINWINDOW_H
