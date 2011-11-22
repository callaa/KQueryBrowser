#ifndef QUERYVIEW_H
#define QUERYVIEW_H

#include <QWebView>

class QueryResults;

class QueryView : public QWebView
{
    Q_OBJECT
public:
    explicit QueryView(QWidget *parent = 0);

	/**
	  \brief Erase current contents
	  */
	void clear();

	/**
	  \brief Prepare for a new query

	  */
	void startNewQuery(const QString& querystr);

	void showResults(const QueryResults& results);

signals:

public slots:

protected slots:
	void initQueryBrowser(bool ok);

private:
	//! Number of queries performed
	int m_querycount;
};

#endif // QUERYVIEW_H
