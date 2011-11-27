#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>

class Connection;
class QTabWidget;

class MainWindow : public KXmlGuiWindow
{
	Q_OBJECT
public:
	MainWindow(Connection *connection, QWidget *parent=0);
	~MainWindow();

public slots:
	//! Open a new query tab
	void newQueryTab();

	//! Open a new blank script tab
	void newScriptTab();

	//! Open a script file
	void openScript();

	//! Save the contents of the currently open script tab
	void saveScript();

	//! Save the contents of the currently open script tab with a new name
	void saveScriptAs();

	/**
	  \brief Run a query

	  The query is executed in the currently open query tab
	  */
	void runQuery(const QString& query);

protected slots:
	//! Close a tab
	void closeTab(int index);

	//! The "content modified" flag on a tab has changed
	void tabNameChange(const QString& name);

	void currentTabChanged(int index);

private:
	void setupActions();
	void newTab(QWidget *widget, const QString& title);

	QTabWidget *m_tabs;
	Connection *m_connection;
	int m_querytabs;

};

#endif // MAINWINDOW_H