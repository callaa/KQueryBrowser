#include <QUrl>

#include <KMessageBox>

#include "connectiondialog.h"
#include "ui_connectiondialog.h"
#include "mainwindow.h"

#include "sqlite3connection.h"

ConnectionDialog::ConnectionDialog(QWidget *parent) :
	KDialog(parent),
	m_ui(new Ui::ConnectionDialog),
	m_connection(0)
{
	setWindowTitle(tr("Open database connection"));
	QWidget *mainwidget = new QWidget(this);
	m_ui->setupUi(mainwidget);
	setMainWidget(mainwidget);
}

ConnectionDialog *ConnectionDialog::open(const QUrl& url)
{
	ConnectionDialog *dlg = new ConnectionDialog();

	bool ok = false;
	if(url.scheme() == "sqlite3") {
		dlg->m_ui->stackedWidget->setCurrentIndex(0);
		dlg->m_ui->filepath->setText(url.path());
		// TODO check that the file exists
		ok = !url.path().isEmpty();
	} else {
		KMessageBox::sorry(0,"Unknown database type " + url.scheme());
	}

	dlg->show();

	if(ok)
		dlg->openConnection();

	return dlg;
}

ConnectionDialog::~ConnectionDialog()
{
	delete m_ui;
}

void ConnectionDialog::slotButtonClicked(int button)
{
	if(button==KDialog::Ok) {
		openConnection();
	} else {
		KDialog::slotButtonClicked(button);
	}
}

void ConnectionDialog::opened()
{
	QApplication::restoreOverrideCursor();
	MainWindow *mw = new MainWindow(m_connection);
	m_connection = 0;
	mw->show();
	accept();
}

void ConnectionDialog::failed(const QString &message)
{
	QApplication::restoreOverrideCursor();
	delete m_connection;
	enableButtonOk(true);
	enableButtonCancel(true);
	KMessageBox::error(this,tr("Couldn't open database connection!"), message);
}

void ConnectionDialog::openConnection()
{
	// Create a proper type of connection
	if(m_ui->dbtype->currentIndex()==0) {
		m_connection = new Sqlite3Connection(m_ui->filepath->text());
	} else {
		KMessageBox::sorry(this,"Bug: unimplemented selection " + QString::number(m_ui->dbtype->currentIndex()));
		return;
	}

	// Connect open/fail signals
	connect(m_connection, SIGNAL(opened()), this, SLOT(opened()));
	connect(m_connection, SIGNAL(cannotOpen(QString)), this, SLOT(failed(QString)));

	// Prepare for waiting for the connection to open
	enableButtonOk(false);
	enableButtonCancel(false);
	QApplication::setOverrideCursor(Qt::WaitCursor);

	// Go.
	m_connection->start();
}
