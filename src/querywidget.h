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
