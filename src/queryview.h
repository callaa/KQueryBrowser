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
#ifndef QUERYVIEW_H
#define QUERYVIEW_H

#include <QWebView>
#include <QVector>
#include <QVariant>
#include <QPair>

class QueryResults;
class TableCellIterator;

// Pair of column name and data
typedef QPair<QString,QVariant> BigValue;

class QueryView : public QWebView
{
    Q_OBJECT
public:
    explicit QueryView(QWidget *parent = 0);

	//! How many results to return per "page" by default
	static const int DEFAULT_PAGESIZE = 40;

	/**
	  \brief Erase current contents
	  */
	void clear();

	/**
	  \brief Prepare for a new query

	  */
	void startNewQuery(const QString& querystr);

	/**
	  \brief Show the results of a query.
	  */
	void showResults(const QueryResults& results);

	/**
	 * \brief Return an iterator for exporting view contents
	 *
	 * Note. You are responsible for deleting the iterator yourself
	 * after you're done.
	 * \return new table cell iterator instance
	 */
	TableCellIterator *tableIterator() const;

signals:
	void getMoreResults(int limit);

public slots:
	/**
	 * \brief Request more results
	 *
	 * This is typically called from the viewer JavaScript.
	 */
	void queryGetMore();

	/**
	 * \brief Request the rest of the results
	 *
	 * This is typically called from the viewer JavaScript.
	 */
	void queryGetAll();

	/**
	 * \brief Show a long result value in a viewer dialog
	 *
	 * This is typically called from the viewer JavaScript.
	 * \param index index of result in bigresult vector.
	 */
	void showBigResult(int index);

	/**
	 * \brief Export a single table
	 *
	 * This is typically called from the viewer JavaScript.
	 * \param id table ID
	 * \param format export format
	 */
	void exportTable(const QString& id, const QString& format);

protected slots:
	void initQueryBrowser(bool ok);

protected:
	bool event(QEvent *event);

private:
	//! Number of queries performed
	int m_querycount;

	//! Number of results received in the last query
	int m_resultsgot;

	//! Number of result rows expected (-1 if unknown)
	int m_resultsexpected;

	//! Result entries too big to display in the table view
	QVector<BigValue> m_bigresults;

	//! Column names of the last query
	QStringList m_columnnames;
};

#endif // QUERYVIEW_H
