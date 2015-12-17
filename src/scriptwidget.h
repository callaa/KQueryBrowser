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
#include <QUrl>

namespace KTextEditor {
	class View;
	class Document;
}
namespace db {
	class Query;
	class QueryResults;
}
class QueryView;
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
	ScriptWidget(const QUrl &url, QWidget *parent);

	//! Was initialization succesful
	bool isValid() const;

	/**
	 * \brief Set the content of the script document
	 *
	 * \param text the new content
	 */
	void setContent(const QString& text);

	/**
	 * \brief Save the script
	 * \return true on success
	 */
	bool save();

	/**
	 * \brief Save the script with a new name
	 * \return true on success
	 */
	bool saveAs(const QUrl& url);

	const QUrl& documentUrl() const { return m_documenturl; }
	QString documentName() const;
	bool isUnsaved() const;

	TableCellIterator *tableIterator() const;

signals:
	/**
	 * \brief Name of the script has changed
	 * \param name the new name
	 */
	void nameChange(const QString& name);

public slots:
	void attachQueryContext(db::Query *ctx);

	/**
	 * \brief Execute the script
	 */
	void executeQuery();

	/**
	 * \brief Execute the currently selected part of the script.
	 */
	void executeSelection();


	/**
	 * \brief Clear away all query results
	 */
	void clearResults();

private slots:
	void scriptModifiedChanged();
	void selectionChanged();

protected:
	void showEvent(QShowEvent *e);

private:

	QUrl m_documenturl;
	db::Query *m_ctx;
	KTextEditor::Document *m_document;
	KTextEditor::View *m_view;
	QueryView *m_resultview;

	QAction *m_actrun;
	QAction *m_actrunsel;
};

#endif // SCRIPTWIDGET_H
