find_package(Qt5 REQUIRED COMPONENTS
	Core
	Widgets
	Sql
	WebKitWidgets
)
find_package(KF5 REQUIRED COMPONENTS
	CoreAddons
	I18n
	WidgetsAddons
	TextWidgets
	Bookmarks
	ConfigWidgets
	KIO
	Completion
	TextEditor
)
find_package(OktetaGui REQUIRED)

set(kquerybrowser_SRCS
	db/connection.cpp
	db/serverconnection.cpp
	db/query.cpp
	db/queryresults.cpp
	db/backend/sqlite3connection.cpp
	db/backend/mysqlconnection.cpp
	db/backend/pgsqlconnection.cpp
	meta/database.cpp
	meta/schema.cpp
	meta/table.cpp
	meta/column.cpp
	export/exporter.cpp
	export/htmlexporter.cpp
	export/csvexporter.cpp
	main.cpp
	mainwindow.cpp
	connectiondialog.cpp
	queryview.cpp
	valueview.cpp
	querywidget.cpp
	scriptwidget.cpp
	tablelistwidget.cpp
	databaselistwidget.cpp
	sqllineedit.cpp
	stringbuilder.cpp
	bookmarks.cpp
	bookmarkdialog.cpp
	sqlcompletion.cpp
)

SET(kquerybrowser_RCCS
	../res/resources.qrc
)

qt5_wrap_ui(kquerybrowser_SRCS
	ui/connectiondialog.ui
	ui/valueviewwidget.ui
	ui/findwidget.ui
)

qt5_add_resources(kquerybrowser_SRCS ../res/resources.qrc)

add_executable(kquerybrowser ${kquerybrowser_SRCS})

target_link_libraries(kquerybrowser
	Qt5::Widgets
	Qt5::WebKitWidgets
	Qt5::Sql
	KF5::CoreAddons
	KF5::KIOCore
	KF5::TextWidgets
	KF5::Bookmarks
	KF5::ConfigWidgets
	KF5::KIOCore
	KF5::KIOWidgets
	KF5::KIOFileWidgets
	KF5::Completion
	KF5::TextEditor
	OktetaGui
)

install(TARGETS kquerybrowser  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

