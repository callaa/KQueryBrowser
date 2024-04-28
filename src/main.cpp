#include "windows/mainwindow.h"
#include "windows/newconnectiondialog.h"
#include "db/database.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>
#include <QWebEngineUrlScheme>
#include <KAboutData>
#include <KLocalizedString>

using namespace Qt::StringLiterals;

int main(int argc, char **argv)
{
    // Register the Web Engine URL scheme we use to communicate with the views.
    // This has to be done before instatiating QApplication
    QWebEngineUrlScheme apiScheme("api");
    apiScheme.setSyntax(QWebEngineUrlScheme::Syntax::Path);
    apiScheme.setFlags(QWebEngineUrlScheme::SecureScheme | QWebEngineUrlScheme::CorsEnabled | QWebEngineUrlScheme::FetchApiAllowed);
    QWebEngineUrlScheme::registerScheme(apiScheme);

    // Initialize application
	QApplication app(argc, argv);
    KLocalizedString::setApplicationDomain("kquerybrowser");

    // Set application info
    KAboutData aboutData(
        u"kquerybrowser"_s,
        u"KQueryBrowser"_s,
        u"1.0.0"_s,
        i18n("SQL query browser"),
        KAboutLicense::GPL_V3,
        i18n("(c) 2024"),
        i18n("A database query tool for PostgreSQL, MariaDB, and Sqlite3"),
        u"https://github.com/callaa/kquerybrowser/"_s,
        u"https://github.com/callaa/kquerybrowser/issues"_s
    );
    aboutData.addAuthor(u"Calle Laakkonen"_s, QString(), u"laakkonenc@gmail.com"_s);
    KAboutData::setApplicationData(aboutData);

    // Parse command line arguments
    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);

    parser.addPositionalArgument(u"url"_s, i18n("Database URL"));

    parser.process(app);
    aboutData.processCommandLine(&parser);

    // Connect directly to a database if URL is given
    if(!parser.positionalArguments().isEmpty()) {
        auto url = QUrl(parser.positionalArguments().at(0));
        if(!url.isValid()) {
            qFatal() << "Invalid URL" << parser.positionalArguments().at(0);
            return 1;
        }

        auto db = DatabaseAdapter::openConnection(url);
        if(!db) {
            return 1;
        }

        auto mw = new MainWindow(db);
        mw->show();

    } else {
        // Open connection selector
        NewConnectionDialog::openNewDialog();
    }

	return app.exec();
}
