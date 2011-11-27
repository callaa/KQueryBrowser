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

class QueryResults;

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

signals:
	void getMoreResults(int limit);

public slots:
	void queryGetMore();
	void queryGetAll();

protected slots:
	void initQueryBrowser(bool ok);

private:
	//! Number of queries performed
	int m_querycount;

	//! Number of results received in the last query
	int m_resultsgot;

	//! Number of result rows expected (-1 if unknown)
	int m_resultsexpected;
};

#endif // QUERYVIEW_H
