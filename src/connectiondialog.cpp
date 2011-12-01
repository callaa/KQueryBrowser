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
#include <QUrl>

#include <KMessageBox>

#include "connectiondialog.h"
#include "ui_connectiondialog.h"
#include "mainwindow.h"

#include "db/sqlite3connection.h"
#include "db/mysqlconnection.h"
#include "db/pgsqlconnection.h"

struct ConType {
	const char *name;
	int page;
};

static const ConType CONTYPES[] = {
	{"SQLite", 0},
	{"MySQL", 1},
	{"PostgreSQL", 1}
};

ConnectionDialog::ConnectionDialog(QWidget *parent) :
	KDialog(parent),
	m_ui(new Ui::ConnectionDialog),
	m_connection(0)
{
	setWindowTitle(tr("Open database connection"));
	setWindowIcon(KIcon("kquerybrowser"));
	QWidget *mainwidget = new QWidget(this);
	m_ui->setupUi(mainwidget);
	for(unsigned int i=0;i<sizeof CONTYPES / sizeof CONTYPES[0];++i) {
		m_ui->dbtype->addItem(CONTYPES[i].name);
	}
	connect(m_ui->dbtype, SIGNAL(currentIndexChanged(int)), this, SLOT(typeSelected(int)));

	setMainWidget(mainwidget);
}

void ConnectionDialog::typeSelected(int index)
{
	m_ui->stackedWidget->setCurrentIndex(CONTYPES[index].page);
}

ConnectionDialog *ConnectionDialog::open(const QUrl& url)
{
	ConnectionDialog *dlg = new ConnectionDialog();

	bool ok = false;
	if(url.scheme() == "sqlite3") {
		dlg->m_ui->dbtype->setCurrentIndex(0);
		dlg->m_ui->filepath->setText(url.path());
		// TODO check that the file exists
		ok = !url.path().isEmpty();
	} else if(url.scheme()=="mysql" || url.scheme()=="pgsql") {
		dlg->m_ui->dbtype->setCurrentIndex(url.scheme()=="mysql" ? 1 : 2);
		dlg->m_ui->dbname->setText(url.path().mid(1));
		if(url.port()>0)
			dlg->m_ui->serverport->setText(QString::number(url.port()));
		dlg->m_ui->servername->setText(url.host());
		dlg->m_ui->username->setText(url.userName());
		dlg->m_ui->password->setText(url.password());

		ok = !dlg->m_ui->servername->text().isEmpty();
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
	// Build the URL for the connection
	KUrl url;
	if(m_ui->dbtype->currentIndex()==0) {
		url = KUrl("sqlite3:" + m_ui->filepath->text());
	} else {
		if(m_ui->dbtype->currentIndex()==1)
			url.setScheme("mysql");
		else if(m_ui->dbtype->currentIndex()==2)
			url.setScheme("pgsql");
		else  {
			KMessageBox::sorry(this,"Bug: unimplemented selection " + QString::number(m_ui->dbtype->currentIndex()));
			return;
		}

		url.setHost(m_ui->servername->text());
		bool ok;
		int port = m_ui->serverport->text().toInt(&ok);
		if(ok)
			url.setPort(port);
		url.setUserName(m_ui->username->text());
		url.setPassword(m_ui->username->text());
		url.setPath(m_ui->dbname->text());
	}

	// Create the connection
	m_connection = Connection::create(url);
	Q_ASSERT(m_connection);

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
