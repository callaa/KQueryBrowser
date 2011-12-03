project (kquerybrowser)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories(${KDE4_INCLUDES})

set(kquerybrowser_SRCS
	src/db/connection.cpp
	src/db/sqlite3connection.cpp
	src/db/serverconnection.cpp
	src/db/mysqlconnection.cpp
	src/db/pgsqlconnection.cpp
	src/db/dbctxmanager.cpp
	src/db/dbcontext.cpp
	src/db/queryresults.cpp
	src/meta/database.cpp
	src/meta/schema.cpp
	src/meta/table.cpp
	src/meta/column.cpp
	src/export/exporter.cpp
	src/export/htmlexporter.cpp
	src/export/csvexporter.cpp
	src/main.cpp
	src/mainwindow.cpp
	src/connectiondialog.cpp
	src/queryview.cpp
	src/valueview.cpp
	src/querywidget.cpp
	src/scriptwidget.cpp
	src/tablelistwidget.cpp
	src/databaselistwidget.cpp
	src/sqllineedit.cpp
	src/stringbuilder.cpp
	src/bookmarks.cpp
)

SET(kquerybrowser_RCCS
	res/resources.qrc
)

kde4_add_ui_files(kquerybrowser_SRCS
	ui/connectiondialog.ui
	ui/valueviewwidget.ui
)

qt4_add_resources(kquerybrowser_SRCS ${kquerybrowser_RCCS})

kde4_add_executable(kquerybrowser ${kquerybrowser_SRCS})

target_link_libraries(kquerybrowser
	${KDE4_KDEUI_LIBS}
	${KDE4_KIO_LIBS}
	${KDE4_KTEXTEDITOR_LIBS}
	${QT_QTSQL_LIBRARY}
	${QT_QTWEBKIT_LIBRARY}
)

install(TARGETS kquerybrowser  ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES
	data/kquerybrowserui.rc
	data/queryscript.js
	data/querystyle.css
	data/export.html
	data/sep0.png
	data/sep1.png
	data/ajax-loader.gif
	DESTINATION  ${DATA_INSTALL_DIR}/kquerybrowser
)
install(PROGRAMS
	kquerybrowser.desktop
	DESTINATION ${XDG_APPS_INSTALL_DIR}
)

kde4_install_icons(${ICON_INSTALL_DIR})
kde4_update_iconcache()

