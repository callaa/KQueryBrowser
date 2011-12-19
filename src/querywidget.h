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
class TableCellIterator;

namespace Ui {
	class FindWidget;
}

/**
 * \brief A tab widget for interactive querying
 *
 * This widget contains a large, query result widget, a hidable
 * search widget (for searching inside the results)
 * and a line edit box for entering SQL code.
 */
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

	/**
	 * \brief Get a table iterator for exporting results
	 * \return table iterator instance
	 */
	TableCellIterator *tableIterator() const;

signals:
	void doQuery(const QString& query, int limit);
	void getMoreResults(int limit);

public slots:
	/**
	 * \brief Show query results
	 * \param results query results
	 */
	void queryResults(const QueryResults& results);

	/**
	 * \brief Clear results view
	 */
	void clearResults();

	/**
	 * \brief Show the controls for searching in results
	 *
	 */
	void showSearch();

	/**
	 * \brief Find the next match in the result view
	 *
	 * This will display the search bar if it is currently hidden.
	 */
	void findNext();

	/**
	 * \brief Find the previous match in the result view
	 *
	 * This will display the search bar if it is currently hidden.
	 */
	void findPrev();

protected slots:
	void doQuery(const QString& query);

protected:
	void showEvent(QShowEvent *e);

private:
	void findInPage(bool forward);

	QueryView *m_view;
	SqlLineEdit *m_query;
	QWidget *m_find;
	Ui::FindWidget *m_findui;
	bool m_moreavailable;
};

#endif // QUERYWIDGET_H
