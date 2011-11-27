#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <KDialog>

namespace Ui {
    class ConnectionDialog;
}

class Connection;
class QUrl;

class ConnectionDialog : public KDialog
{
    Q_OBJECT

public:
    explicit ConnectionDialog(QWidget *parent = 0);

	static ConnectionDialog *open(const QUrl& url);

    ~ConnectionDialog();

protected slots:
	void typeSelected(int index);
	void slotButtonClicked(int button);
	void opened();
	void failed(const QString& message);

private:
	void openConnection();

	Ui::ConnectionDialog *m_ui;
	Connection *m_connection;
};

#endif // CONNECTIONDIALOG_H
