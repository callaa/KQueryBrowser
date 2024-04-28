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
#ifndef TABLELIST_H
#define TABLELIST_H

#include <QDockWidget>

class QTreeWidget;

namespace dbschema {
	class Database;
}

class TableListDock : public QDockWidget
{
    Q_OBJECT
public:
    explicit TableListDock(QWidget *parent=nullptr);

Q_SIGNALS:
	//! Request a query to be run in the active query view
    void queryRequested(const QString& query);

    //! Request database schema refresh
    void refreshRequested();

public Q_SLOTS:
    void setSchema(const dbschema::Database& db);

protected Q_SLOTS:
	void customContextMenu(const QPoint& point);

private:
	QTreeWidget *m_view;

    //bool m_canshowcreate;
};

#endif

