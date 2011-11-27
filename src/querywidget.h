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
#ifndef QUERYWIDGET_H
#define QUERYWIDGET_H

#include <QWidget>

class QueryView;
class SqlLineEdit;
class QueryResults;

class QueryWidget : public QWidget
{
    Q_OBJECT
public:
    explicit QueryWidget(QWidget *parent = 0);

	/**
	  \brief Execute a query in this query widget.

	  The query will be pushed to the entry box history, so the user
	  can modify and re-run it.

	  \param query the query to run
	  */
	void runQuery(const QString& query);

signals:
	void doQuery(const QString& query, int limit);
	void getMoreResults(int limit);

public slots:
	void queryResults(const QueryResults& results);

protected slots:

	void doQuery(const QString& query);

protected:
	void showEvent(QShowEvent *e);

private:
	QueryView *m_view;
	SqlLineEdit *m_query;
	bool m_moreavailable;
};

#endif // QUERYWIDGET_H
