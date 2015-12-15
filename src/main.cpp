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
#include "connectiondialog.h"
#include "db/queryresults.h"
#include "meta/database.h"
#include "bookmarks.h"

#include <QApplication>
#include <QUrl>
#include <QCommandLineParser>
#include <QDebug>

#include <KAboutData>
#include <KLocalizedString>

void initApp(QApplication &app)
{
	qRegisterMetaType<db::QueryResults>();
	qRegisterMetaType<meta::Database>();

	KAboutData aboutData(
		// The program name used internally.
		QStringLiteral("kquerybrowser"),
		// A displayable program name string.
		i18n("KQueryBrowser"),
		// The program version string.
		QStringLiteral("0.2.0"),
		// Short description of what the app does.
		i18n("A database query tool for KDE"),
		// The license this code is released under
		KAboutLicense::GPL_V3,
		// Copyright Statement
		QStringLiteral("(c) 2011-2015, Calle Laakkonen"),
		// Optional text shown in the About box.
		// Can contain any information desired.
		QString(),
		// The program homepage string.
		QStringLiteral("https://github.com/callaa/kquerybrowser/"),
		// The bug report email address
		QStringLiteral("laakkonenc@gmail.com")
	);

    KAboutData::setApplicationData(aboutData);

	Bookmarks::init();

	QCommandLineParser parser;
	parser.setApplicationDescription("A SQL query browser for KDE");
	parser.addHelpOption();
	parser.addPositionalArgument("URL", "Database URL");

	parser.process(app);

	QStringList args = parser.positionalArguments();
	if(args.isEmpty()) {
		(new ConnectionDialog())->show();

	} else {
		for(const QString &arg : args) {
			QUrl url(arg);
			if(url.isValid()) {
				ConnectionDialog::openDialog(url);
			} else {
				qWarning() << "Invalid URL:" << arg;
			}
		}
	}

}

int main (int argc, char *argv[])
{
	QApplication app(argc, argv);

	initApp(app);

	return app.exec();
}
