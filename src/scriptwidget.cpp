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
#include <QDebug>

#include <QAction>
#include <QToolBar>
#include <QSplitter>
#include <QVBoxLayout>
#include <QRegExp>

#include <KTextEditor/EditorChooser>
#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <KTextEditor/ConfigInterface>

#include <KMessageBox>

#include "scriptwidget.h"
#include "queryview.h"

ScriptWidget::ScriptWidget(const KUrl& url, QWidget *parent) :
	QWidget(parent), m_documenturl(url), m_document(0)
{
	// Load document
	KTextEditor::Editor *editor = KTextEditor::EditorChooser::editor();
	if (!editor) {
		KMessageBox::error(0, tr("A KDE text-editor component could not be found;\n" "please check your KDE installation."));
		return;
	}

	m_document = editor->createDocument(this);
	if(!url.isEmpty()) {
		if(!m_document->openUrl(url)) {
			return;
		}
	}

	connect(m_document, SIGNAL(modifiedChanged(KTextEditor::Document*)), this, SLOT(scriptModifiedChanged()));
	connect(m_document, SIGNAL(documentNameChanged(KTextEditor::Document*)), this, SLOT(scriptModifiedChanged()));

	// Create text editor view
	m_view = m_document->createView(this);

	// Set SQL highlight mode (TODO mysql/postgresql variant)
	/* TODO find the best matching mode
	QStringList hilites = m_document->highlightingModes();
	int hilitemode = hilites.indexOf(QRegExp("mysql", Qt::CaseInsensitive));
	*/
	m_document->setHighlightingMode("sql");

	// Set view options
	KTextEditor::ConfigInterface *viewcfg = qobject_cast<KTextEditor::ConfigInterface*>(m_view);
	if(viewcfg!=0) {
		viewcfg->setConfigValue("line-numbers", true);
	}

	// Result view
	m_resultview = new QueryView(this);

	// Action bar
	QToolBar *toolbar = new QToolBar(this);
	toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	toolbar->setIconSize(QSize(16,16));

	// Actions
	QAction *actExec = new QAction(KIcon("quickopen"), tr("Run"), this);
	connect(actExec, SIGNAL(triggered()), this, SLOT(executeQuery()));
	toolbar->addAction(actExec);

#if 0 // todo
	QAction *actStep = new QAction(KIcon("debug-step-over"), tr("Step"), this);
	toolbar->addAction(actStep);

	QAction *actStop = new QAction(KIcon("process-stop"), tr("Stop"), this);
	actStop->setEnabled(false);
	toolbar->addAction(actStop);
#endif

	// Layout
	QSplitter *splitter = new QSplitter(Qt::Vertical, this);
	splitter->setChildrenCollapsible(false);
	splitter->addWidget(m_view);
	splitter->addWidget(m_resultview);
	splitter->setStretchFactor(0, 30);
	splitter->setStretchFactor(1, 1);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->addWidget(toolbar);
	layout->addWidget(splitter);
	setLayout(layout);
}

bool ScriptWidget::isValid() const
{
	return m_document != 0;
}

void ScriptWidget::executeQuery()
{
	m_resultview->clear();
	m_resultview->startNewQuery(QString());

	// TODO this only executes the first statement on sqlite
	emit doQuery(m_document->text(), 0);
}

void ScriptWidget::queryResults(const QueryResults& results)
{
	m_resultview->showResults(results);
}

void ScriptWidget::clearResults()
{
	m_resultview->clear();
}

TableCellIterator *ScriptWidget::tableIterator() const
{
	return m_resultview->tableIterator();
}

bool ScriptWidget::save()
{
	return m_document->save();
}

bool ScriptWidget::saveAs(const KUrl& url)
{
	if(m_document->saveAs(url)) {
		m_documenturl = url;
		return true;
	} else {
		return false;
	}
}

QString ScriptWidget::documentName() const
{
	QString name = m_document->documentName();
	if(m_document->isModified())
		name += "*";
	return name;
}

bool ScriptWidget::isUnsaved() const
{
	return m_document->isModified();
}

void ScriptWidget::scriptModifiedChanged()
{
	emit nameChange(documentName());
}

void ScriptWidget::showEvent(QShowEvent *e)
{
	m_view->setFocus();
	QWidget::showEvent(e);
}
