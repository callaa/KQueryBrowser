#ifndef SCRIPTWIDGET_H
#define SCRIPTWIDGET_H

#include <QWidget>
#include <KUrl>

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
	ScriptWidget(const KUrl &url, QWidget *parent);

	//! Was initialization succesful
	bool isValid() const;

	bool save();
	bool saveAs(const KUrl& url);

	const KUrl& documentUrl() const { return m_documenturl; }
	QString documentName() const;
	bool isUnsaved() const;

signals:
	void doQuery(const QString& query, int limit);
	void getMoreResults(int limit);
	void nameChange(const QString& name);

public slots:
	void executeQuery();
	void queryResults(const QueryResults& results);

protected slots:
	void scriptModifiedChanged();

protected:
	void showEvent(QShowEvent *e);

private:

	KUrl m_documenturl;
	KTextEditor::Document *m_document;
	KTextEditor::View *m_view;
	QueryView *m_resultview;
};

#endif // SCRIPTWIDGET_H
