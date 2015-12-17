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

#include "scriptwidget.h"
#include "queryview.h"
#include "db/query.h"

#include <QDebug>

#include <QAction>
#include <QToolBar>
#include <QSplitter>
#include <QVBoxLayout>
#include <QRegExp>

#include <KMessageBox>
#include <KTextEditor/Editor>
#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <KTextEditor/ConfigInterface>

ScriptWidget::ScriptWidget(const QUrl& url, QWidget *parent) :
	QWidget(parent), m_documenturl(url), m_ctx(nullptr), m_document(nullptr)
{
	// Load document
	KTextEditor::Editor *editor = KTextEditor::Editor::instance();
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
	connect(m_view, SIGNAL(selectionChanged(KTextEditor::View*)),
			this, SLOT(selectionChanged()));

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
	m_actrun = new QAction(QIcon::fromTheme("debug-run"), tr("Run"), this);
	m_actrun->setToolTip(tr("Run the whole script"));
	connect(m_actrun, &QAction::triggered, this, &ScriptWidget::executeQuery);
	toolbar->addAction(m_actrun);

	m_actrunsel = new QAction(QIcon::fromTheme("debug-run-cursor"), tr("Run selection"), this);
	m_actrunsel->setToolTip(tr("Run just the selected part of the script"));
	connect(m_actrunsel, &QAction::triggered, this, &ScriptWidget::executeSelection);
	m_actrunsel->setEnabled(false);
	toolbar->addAction(m_actrunsel);

#if 0 // todo
	QAction *actStep = new QAction(QIcon::fromTheme("debug-step-over"), tr("Step"), this);
	toolbar->addAction(actStep);

	QAction *actStop = new QAction(QIcon::fromTheme("process-stop"), tr("Stop"), this);
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

void ScriptWidget::setContent(const QString& content)
{
	m_document->setText(content);
}

void ScriptWidget::attachQueryContext(db::Query *ctx)
{
	m_ctx = ctx;
	connect(this, &QObject::destroyed, m_ctx, &QObject::deleteLater);
	connect(m_ctx, &db::Query::results, m_resultview, &QueryView::showResults);
}

void ScriptWidget::executeQuery()
{
	if(!m_ctx) {
		qWarning() << "Query context not attached!";
		return;
	}

	m_resultview->clear();
	m_resultview->startNewQuery(QString());

	// TODO this only executes the first statement on sqlite
	m_ctx->query(m_document->text(), 0);
}

void ScriptWidget::executeSelection()
{
	if(!m_ctx) {
		qWarning() << "Query context not attached!";
		return;
	}

	m_resultview->clear();
	m_resultview->startNewQuery(QString());

	// TODO this only executes the first statement on sqlite
	m_ctx->query(m_view->selectionText(), 0);
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

bool ScriptWidget::saveAs(const QUrl& url)
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

void ScriptWidget::selectionChanged()
{
	m_actrunsel->setEnabled(m_view->selection());
}


void ScriptWidget::showEvent(QShowEvent *e)
{
	m_view->setFocus();
	QWidget::showEvent(e);
}

