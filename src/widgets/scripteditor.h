#ifndef SCRIPTEDITOR_H
#define SCRIPTEDITOR_H

#include <QWidget>

namespace KTextEditor {
    class View;
    class Document;
}

class QWebEngineProfile;

class DatabaseAdapter;
class QueryView;
class WebApi;

class ScriptEditor : public QWidget
{
    Q_OBJECT
public:
    explicit ScriptEditor(const QUrl &documentUrl, DatabaseAdapter *db, WebApi *webapi, QWebEngineProfile *profile, QWidget *parent = nullptr);
    ~ScriptEditor();

    QUrl documentUrl() const;
    QString documentName() const;
    bool isUnsaved() const;

    bool save();
    bool saveAs(const QUrl &url);

    void clearResults();
    void runScript();
    void runSelectedScript();

Q_SIGNALS:
    //! Emitted when the document's "modified" status has changed, or the document has changed name
    void modifiedChanged();

    //! Emitted when the "run selection" action availability changes
    void runnableSelectionChanged(bool selected);

private:
    KTextEditor::Document *m_document;
    KTextEditor::View *m_view;
    QueryView *m_resultview;
};

#endif // SCRIPTEDITOR_H
