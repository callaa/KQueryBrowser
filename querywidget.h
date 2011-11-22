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

signals:
	void doQuery(const QString& query, int limit);

public slots:
	void queryResults(const QueryResults& results);

protected slots:

	void doQuery(const QString& query);

private:
	QueryView *m_view;
	SqlLineEdit *m_query;
};

#endif // QUERYWIDGET_H
