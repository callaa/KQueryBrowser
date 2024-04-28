#ifndef NEWCONNECTIONDIALOG_H
#define NEWCONNECTIONDIALOG_H

#include <QDialog>
#include <KBookmarkOwner>

class Ui_newConnection;
class KBookmark;

class NewConnectionDialog : public QDialog, KBookmarkOwner
{
    Q_OBJECT
public:
    NewConnectionDialog();
    ~NewConnectionDialog();

    //! Return the full connection string
    QString connectionString() const;

    /**
     * Return the connection string as a QUrl for saving in a bookmark
     *
     * TODO: the password should be omitted from here and instead saved into the wallet
     */
    QUrl currentUrl() const override;
    QString currentTitle() const override;
    void openBookmark(const KBookmark &bm, Qt::MouseButtons mb, Qt::KeyboardModifiers km) override;

    static void openNewDialog(const QString &initialUrl=QString());

private Q_SLOTS:
    void updateFromUrl();
    void updateSqliteUrl();
    void updateGenericUrl();

private:
    void updateSqliteFromUrl(const QUrl &url);
    void updateGenericFromUrl(const QUrl &url);

    Ui_newConnection *m_ui;
};

#endif // NEWCONNECTIONDIALOG_H
