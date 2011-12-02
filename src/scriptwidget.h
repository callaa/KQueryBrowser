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
class TableCellIterator;

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

	TableCellIterator *tableIterator() const;

signals:
	void doQuery(const QString& query, int limit);
	void getMoreResults(int limit);
	void nameChange(const QString& name);

public slots:
	void executeQuery();
	void queryResults(const QueryResults& results);
	void clearResults();

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
