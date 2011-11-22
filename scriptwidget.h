#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include <QWidget>

namespace KTextEditor {
	class View;
	class Document;
}

class QueryView;
class QueryResults;

/**
  \brief An SQL script editor widget

  The script widget offers a superset of query widget's functionality, but unlike QueryWidget which
  is meant for interactive querying, ScriptWidget is intended for testing and running complex queries and scripts.
  */
class ScriptWidget : public QWidget
{
    Q_OBJECT
public:
	explicit ScriptWidget(QWidget *parent = 0);

signals:
	void doQuery(const QString& query, int limit);

public slots:
	void executeQuery();
	void queryResults(const QueryResults& results);

private:
	KTextEditor::Document *m_document;
	KTextEditor::View *m_view;
	QueryView *m_resultview;
};

#endif // SCRIPTWIDGET_H
