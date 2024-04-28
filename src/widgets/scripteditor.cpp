#include "scripteditor.h"
#include "queryview/queryview.h"

#include <QSplitter>
#include <QVBoxLayout>
#include <KTextEditor/Editor>
#include <KTextEditor/View>
#include <KTextEditor/Document>

using namespace Qt::StringLiterals;

ScriptEditor::ScriptEditor(const QUrl &documentUrl, DatabaseAdapter *db, WebApi *webapi, QWebEngineProfile *profile, QWidget *parent)
    : QWidget{parent}
{
    Q_ASSERT(db);
    Q_ASSERT(webapi);
    Q_ASSERT(profile);

    KTextEditor::Editor *editor = KTextEditor::Editor::instance();
    m_document = editor->createDocument(this);

    if(!documentUrl.isEmpty()) {
        m_document->openUrl(documentUrl);
    }

    connect(m_document, &KTextEditor::Document::modifiedChanged, this, &ScriptEditor::modifiedChanged);
    connect(m_document, &KTextEditor::Document::documentNameChanged, this, &ScriptEditor::modifiedChanged);

    // Create text editor view
    m_view = m_document->createView(this);
    connect(m_view, &KTextEditor::View::selectionChanged, this, [this]() {
        Q_EMIT runnableSelectionChanged(m_view->selection());
    });

    // Set SQL highlight mode
    /* TODO find the best matching mode
        QStringList hilites = m_document->highlightingModes();
        int hilitemode = hilites.indexOf(QRegExp("mysql", Qt::CaseInsensitive));
        */
    m_document->setHighlightingMode(u"sql"_s);

    // Result view
    m_resultview = new QueryView(db, webapi, profile, false, this);

    // Layout
    QSplitter *splitter = new QSplitter(Qt::Vertical, this);
    splitter->setChildrenCollapsible(false);
    splitter->addWidget(m_view);
    splitter->addWidget(m_resultview);
#if 0
    splitter->setStretchFactor(0, 2);
    splitter->setStretchFactor(1, 1);
#endif

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->addWidget(splitter);
    setLayout(layout);
}

ScriptEditor::~ScriptEditor()
{
    // KDE bug? These have to be deleted in this specific order or the application will crash
    delete m_view;
    delete m_document;
}

QUrl ScriptEditor::documentUrl() const
{
    return m_document->url();
}

QString ScriptEditor::documentName() const
{
    QString name = m_document->documentName();
    if(m_document->isModified())
        name.append(u'*');
    return name;
}

bool ScriptEditor::isUnsaved() const
{
    return m_document->isModified();
}

bool ScriptEditor::save()
{
    return m_document->save();
}

bool ScriptEditor::saveAs(const QUrl &url)
{
    return m_document->saveAs(url);
}

void ScriptEditor::clearResults()
{
    m_resultview->clearResults();
}

void ScriptEditor::runScript()
{
    m_resultview->clearResults();
    m_resultview->makeQuery(m_document->text());
}

void ScriptEditor::runSelectedScript()
{
    QString selection = m_view->selectionText();
    qInfo() << "selection" << selection;
    if(!selection.isEmpty()) {
        m_resultview->clearResults();
        m_resultview->makeQuery(selection);
    }
}
