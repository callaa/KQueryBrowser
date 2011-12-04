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
#ifndef TABLELISTWIDGET_H
#define TABLELISTWIDGET_H

#include <QDockWidget>

class QTreeWidget;
class Database;

class TableListWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit TableListWidget(bool canshowcreate, QWidget *parent = 0);

signals:
	void runQuery(const QString& query);
	void showCreate(const QString& table);
	void refresh();

public slots:
	void refreshTree(const Database& db);

protected slots:
	void customContextMenu(const QPoint& point);

private:
	QTreeWidget *m_view;
	bool m_canshowcreate;
};

#endif // TABLELISTWIDGET_H
