#include "newconnectiondialog.h"
#include "mainwindow.h"
#include "db/database.h"
#include "utils/bookmarks.h"

#include "ui_newconnection.h"

#include <QMenu>
#include <QMessageBox>
#include <KBookmarkMenu>
#include <KLocalizedString>

using namespace Qt::StringLiterals;

// the indices of the database types in the dropdown box
static const int DBTYPE_SQLITE = 0;
static const int DBTYPE_POSTGRES = 1;
static const int DBTYPE_MYSQL = 2;

// the indices of the stacked input widgets
static const int WIDGETS_SQLITE = 0;
static const int WIDGETS_GENERIC = 1;

NewConnectionDialog::NewConnectionDialog()
    : m_ui(new Ui_newConnection)
{
    m_ui->setupUi(this);

    QMenu *bmm = new QMenu(this);
    new KBookmarkMenu(sharedBookmarkManager(), this, bmm);
    m_ui->bookmarkButton->setMenu(bmm);

    connect(m_ui->dbType, &QComboBox::currentIndexChanged, this, [this](int idx) {
        int w = 0;
        switch(idx) {
        case DBTYPE_SQLITE: w = WIDGETS_SQLITE; break;
        case DBTYPE_POSTGRES:
        case DBTYPE_MYSQL: w = WIDGETS_GENERIC; break;
        }
        m_ui->stackedWidget->setCurrentIndex(w);
    });

    connect(m_ui->url, &QLineEdit::textEdited, this, &NewConnectionDialog::updateFromUrl);

    connect(m_ui->sqlitePath, &KUrlRequester::textEdited, this, &NewConnectionDialog::updateSqliteUrl);
    connect(m_ui->sqlitePath, &KUrlRequester::urlSelected, this, &NewConnectionDialog::updateSqliteUrl);

    connect(m_ui->genericHost, &QLineEdit::textEdited, this, &NewConnectionDialog::updateGenericUrl);
    connect(m_ui->genericPort, &QLineEdit::textEdited, this, &NewConnectionDialog::updateGenericUrl);
    connect(m_ui->genericUser, &QLineEdit::textEdited, this, &NewConnectionDialog::updateGenericUrl);
    connect(m_ui->genericPassword, &QLineEdit::textEdited, this, &NewConnectionDialog::updateGenericUrl);
    connect(m_ui->genericDatabase, &QLineEdit::textEdited, this, &NewConnectionDialog::updateGenericUrl);
#if 0
    connect(m_ui->genericRequireSsl, &QCheckBox::clicked, this, &NewConnectionDialog::updateGenericUrl);
#endif
}

NewConnectionDialog::~NewConnectionDialog()
{
    delete m_ui;
}

void NewConnectionDialog::openBookmark(const KBookmark &bm, Qt::MouseButtons mb, Qt::KeyboardModifiers km)
{
    Q_UNUSED(mb);
    Q_UNUSED(km);
    m_ui->url->setText(bm.url().toString());
    updateFromUrl();
}

void NewConnectionDialog::openNewDialog(const QString &initialUrl)
{
    auto dlg = new NewConnectionDialog;
    dlg->setAttribute(Qt::WA_DeleteOnClose);

    dlg->m_ui->url->setText(initialUrl);
    dlg->updateFromUrl();

    QObject::connect(dlg->m_ui->buttonBox, &QDialogButtonBox::accepted, dlg, [dlg]() {
        const QUrl url{dlg->connectionString()};
        if(!url.isValid()) {
            QMessageBox::warning(nullptr, i18n("New Connection"), i18n("Invalid URL"));
            return;
        }

        auto db = DatabaseAdapter::openConnection(url);
        if(!db) {
            QMessageBox::warning(nullptr, i18n("New Connection"), i18n("Unknown database type"));
            return;
        }

        if(!db->isOpen()) {
            QMessageBox::warning(nullptr, i18n("New Connection"), db->databaseError());
            return;
        }

        auto mw = new MainWindow(db);
        mw->show();
        dlg->accept();
    });

    dlg->show();
}

QUrl NewConnectionDialog::currentUrl() const
{
    // TODO we should save the password in a wallet rather than include it in the URL
    return QUrl(m_ui->url->text());
}

QString NewConnectionDialog::currentTitle() const
{
    switch(m_ui->dbType->currentIndex()) {
    case DBTYPE_SQLITE: return m_ui->sqlitePath->text(); // TODO extract just the filename
    case DBTYPE_POSTGRES:
    case DBTYPE_MYSQL: return m_ui->genericDatabase->text();
    }

    return QString();
}

QString NewConnectionDialog::connectionString() const
{
    return m_ui->url->text();
}

void NewConnectionDialog::updateFromUrl()
{
    QUrl url(m_ui->url->text());
    if(url.isValid()) {
        if(url.scheme() == u"sqlite"_s) {
            m_ui->dbType->setCurrentIndex(DBTYPE_SQLITE);
            updateSqliteFromUrl(url);
        } else if(url.scheme() == u"postgres" || url.scheme() == u"postgresql") {
            m_ui->dbType->setCurrentIndex(DBTYPE_POSTGRES);
            updateGenericFromUrl(url);
        } else if(url.scheme() == u"mysql" || url.scheme() == u"mariadb") {
            m_ui->dbType->setCurrentIndex(DBTYPE_MYSQL);
            updateGenericFromUrl(url);
        }
    }
}

void NewConnectionDialog::updateSqliteFromUrl(const QUrl &url)
{
    m_ui->sqlitePath->setText(url.path());
}

void NewConnectionDialog::updateSqliteUrl()
{
    m_ui->url->setText(u"sqlite:%1"_s.arg(m_ui->sqlitePath->text()));
}

void NewConnectionDialog::updateGenericFromUrl(const QUrl &url)
{
    m_ui->genericHost->setText(url.host());

    const int port = url.port();
    if(port<0)
        m_ui->genericPort->setText(QString());
    else
        m_ui->genericPort->setText(QString::number(port));

    m_ui->genericUser->setText(url.userName());
    m_ui->genericPassword->setText(url.password());
    m_ui->genericDatabase->setText(url.path().mid(1));
}

void NewConnectionDialog::updateGenericUrl()
{
    QUrl url;
    switch(m_ui->dbType->currentIndex()) {
    case DBTYPE_POSTGRES: url.setScheme(u"postgres"_s); break;
    case DBTYPE_MYSQL: url.setScheme(u"mysql"_s); break;
    }

    if(!m_ui->genericHost->text().isEmpty())
        url.setHost(m_ui->genericHost->text());

    if(!m_ui->genericPort->text().isEmpty()) {
        bool ok;
        const int port = m_ui->genericPort->text().toInt(&ok);
        if(ok && port >= 0 && port <= 0xffff)
            url.setPort(port);
    }

    if(!m_ui->genericUser->text().isEmpty())
        url.setUserName(m_ui->genericUser->text());

    if(!m_ui->genericPassword->text().isEmpty())
        url.setPassword(m_ui->genericPassword->text());


    if(!m_ui->genericDatabase->text().isEmpty())
        url.setPath(u'/' + m_ui->genericDatabase->text());

#if 0
    if(m_ui->genericRequireSsl->isChecked())
        url.setQuery(u"sslmode=require"_s);
#endif

    m_ui->url->setText(url.toString());
}
