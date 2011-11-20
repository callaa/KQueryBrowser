#ifndef QUERYWIDGET_H
#define QUERYWIDGET_H

#include <QWidget>

class QWebView;
class KLineEdit;
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
	void initQueryBrowser(bool ok);
	void doQuery();

private:
	QWebView *m_view;
	KLineEdit *m_query;
};

#endif // QUERYWIDGET_H
