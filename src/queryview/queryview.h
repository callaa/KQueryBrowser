#ifndef QUERYVIEW_H
#define QUERYVIEW_H

#include <QWidget>
#include <QJsonDocument>

class QWebEngineProfile;
class QWebEngineView;
class QJsonObject;
class SqlLineEdit;
class FindWidget;
class DatabaseAdapter;
class QueryResult;
class WebApi;

class QueryView : public QWidget {
	Q_OBJECT
public:
    QueryView(DatabaseAdapter *db, WebApi *webapi, QWebEngineProfile *profile, bool showInputLine, QWidget *parent=nullptr);
    ~QueryView();

public Q_SLOTS:
    void makeQuery(const QString &query);
    void clearResults();
    void find();
    void findNext();
    void findPrev();

private Q_SLOTS:
    void notifyNewResults();

private:
    Q_INVOKABLE QJsonDocument getResults(const QJsonObject &kwargs);
    Q_INVOKABLE QJsonDocument fetchMoreResults(const QJsonObject &kwargs);
    Q_INVOKABLE QJsonDocument stopResults(const QJsonObject &kwargs);

    void stopResults();

	QWebEngineView *m_view;
    DatabaseAdapter *m_db;
    SqlLineEdit *m_line;
    FindWidget *m_finder;
    QueryResult *m_currentResult;
    bool m_firstResult;
    int m_viewId;

    static int VIEWS;
};

#endif
