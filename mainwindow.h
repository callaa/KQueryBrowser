#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <KXmlGuiWindow>

class Connection;

class MainWindow : public KXmlGuiWindow
{
public:
	MainWindow(Connection *connection, QWidget *parent=0);

private:
	void setupActions();

	Connection *m_connection;

};

#endif // MAINWINDOW_H
