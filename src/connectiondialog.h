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
