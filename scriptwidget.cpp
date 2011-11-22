#include <KTextEditor/EditorChooser>
#include <KTextEditor/View>
#include <KTextEditor/Document>
#include <KTextEditor/ConfigInterface>

#include <KMessageBox>

#include <QAction>
#include <QToolBar>
#include <QSplitter>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QRegExp>

#include "scriptwidget.h"
#include "queryview.h"

ScriptWidget::ScriptWidget(QWidget *parent) :
    QWidget(parent)
{
	// Code editor
	KTextEditor::Editor *editor = KTextEditor::EditorChooser::editor();
	if (!editor) {
		KMessageBox::error(this, tr("A KDE text-editor component could not be found;\n" "please check your KDE installation."));
		return;
	}

	m_document = editor->createDocument(0);
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

	// Actions
	QAction *actExec = new QAction(KIcon("quickopen"), tr("Run"), this);
	connect(actExec, SIGNAL(triggered()), this, SLOT(executeQuery()));

	// Action bar
	QToolBar *toolbar = new QToolBar(this);
	toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
	toolbar->addAction(actExec);

	// Layout
	QSplitter *splitter = new QSplitter(Qt::Vertical, this);
	splitter->setChildrenCollapsible(false);
	splitter->addWidget(m_view);
	splitter->addWidget(m_resultview);
	splitter->setStretchFactor(0, 30);
	splitter->setStretchFactor(1, 1);

	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->addWidget(toolbar);
	layout->addWidget(splitter);
	setLayout(layout);
}

void ScriptWidget::executeQuery()
{
	m_resultview->clear();
	m_resultview->startNewQuery(QString());

	// TODO this only executes the first statement
	emit doQuery(m_document->text(), 0);
}

void ScriptWidget::queryResults(const QueryResults& results)
{
	m_resultview->showResults(results);
}
