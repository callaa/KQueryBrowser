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
#include <QUrl>

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KCmdLineOptions>
#include <KMessageBox>
#include <KLocale>

#include "connectiondialog.h"
#include "db/queryresults.h"
#include "meta/database.h"
#include "bookmarks.h"

int main (int argc, char *argv[])
{
	qRegisterMetaType<QueryResults>();
	qRegisterMetaType<Database>();

	KAboutData aboutData(
				// The program name used internally.
				"kquerybrowser",
				// The message catalog name
				0,
				// A displayable program name string.
				ki18n("KQueryBrowser"),
				// The program version string.
				"0.1.0",
				// Short description of what the app does.
				ki18n("A database query tool for KDE"),
				// The license this code is released under
				KAboutData::License_GPL_V3,
				// Copyright Statement
				ki18n("(c) 2011, Calle Laakkonen"),
				// Optional text shown in the About box.
				// Can contain any information desired.
				ki18n(""),
				// The program homepage string.
				"http://luolamies.org/software/kquerybrowser",
				// The bug report email address
				"calle@luolamies.org");

	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineOptions opts;
	opts.add("+[url]", ki18n("Database URL"));

	KCmdLineArgs::addCmdLineOptions(opts);

	KApplication app;

	Bookmarks::init();

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if(args->count()==0) {
		(new ConnectionDialog())->show();
	} else {
		for(int i=0;i<args->count();++i) {
			QUrl url(args->arg(i));
			if(url.isValid()) {
				ConnectionDialog::open(url);
			}
		}
	}

	return app.exec();
}
