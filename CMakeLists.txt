project (kquerybrowser)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories(${KDE4_INCLUDES})

set(kquerybrowser_SRCS
	src/main.cpp
	src/mainwindow.cpp
	src/connectiondialog.cpp
	src/db/connection.cpp
	src/db/sqlite3connection.cpp
	src/db/serverconnection.cpp
	src/db/mysqlconnection.cpp
	src/db/pgsqlconnection.cpp
	src/db/dbctxmanager.cpp
	src/db/dbcontext.cpp
	src/queryview.cpp
	src/querywidget.cpp
	src/scriptwidget.cpp
	src/queryresults.cpp
	src/database.cpp
	src/schema.cpp
	src/table.cpp
	src/column.cpp
	src/tablelistwidget.cpp
	src/sqllineedit.cpp
	src/stringbuilder.cpp
)

SET(kquerybrowser_RCCS
	res/resources.qrc
)

kde4_add_ui_files(kquerybrowser_SRCS
	ui/connectiondialog.ui
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
	data/sep0.png
	data/sep1.png
	data/ajax-loader.gif
	DESTINATION  ${DATA_INSTALL_DIR}/kquerybrowser
)
install(PROGRAMS
	kquerybrowser.desktop
	DESTINATION ${XDG_APPS_INSTALL_DIR}
)

