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

public slots:
	//! Open a new query tab
	void newQueryTab();

	//! Open a new blank script tab
	void newScriptTab();

	//! Close a tab
	void closeTab(int index);

private:
	void setupActions();

	QTabWidget *m_tabs;
	Connection *m_connection;
	int m_querytabs;

};

#endif // MAINWINDOW_H
