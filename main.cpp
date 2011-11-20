#include <QDebug>
#include <QUrl>

#include <KApplication>
#include <KAboutData>
#include <KCmdLineArgs>
#include <KCmdLineOptions>
#include <KMessageBox>
#include <KLocale>

#include "connectiondialog.h"
#include "queryresults.h"
#include "database.h"

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
				ki18n("(c) 2011"),
				// Optional text shown in the About box.
				// Can contain any information desired.
				ki18n("Some text..."),
				// The program homepage string.
				"http://luolamies.org/",
				// The bug report email address
				"calle@luolamies.org");

	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineOptions opts;
	opts.add("+[url]", ki18n("Database URl"));

	KCmdLineArgs::addCmdLineOptions(opts);

	KApplication app;

	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if(args->count()==0) {
		qDebug() << "Showing";
		(new ConnectionDialog())->show();
	} else {
		for(int i=0;i<args->count();++i) {
			qDebug() << "Opening" << args->arg(i);
			QUrl url(args->arg(i));
			if(url.isValid()) {
				ConnectionDialog::open(url);
			}
		}
	}

	return app.exec();
}
