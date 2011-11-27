project (kquerybrowser)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories(${KDE4_INCLUDES})

set(kquerybrowser_SRCS
	main.cpp
	mainwindow.cpp
	connectiondialog.cpp
	db/connection.cpp
	db/sqlite3connection.cpp
	db/serverconnection.cpp
	db/mysqlconnection.cpp
	db/pgsqlconnection.cpp
	db/dbctxmanager.cpp
	db/dbcontext.cpp
	queryview.cpp
	querywidget.cpp
	scriptwidget.cpp
	queryresults.cpp
	database.cpp
	schema.cpp
	table.cpp
	column.cpp
	tablelistwidget.cpp
	sqllineedit.cpp
	stringbuilder.cpp
)

SET(kquerybrowser_RCCS res/resources.qrc)

kde4_add_ui_files( kquerybrowser_SRCS connectiondialog.ui )

qt4_add_resources(kquerybrowser_SRCS ${kquerybrowser_RCCS})

kde4_add_executable(kquerybrowser ${kquerybrowser_SRCS})

target_link_libraries(kquerybrowser ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${KDE4_KTEXTEDITOR_LIBS} ${QT_QTSQL_LIBRARY} ${QT_QTWEBKIT_LIBRARY})
install(TARGETS kquerybrowser  ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES data/kquerybrowserui.rc data/queryscript.js data/querystyle.css
	data/sep0.png data/sep1.png data/ajax-loader.gif
	DESTINATION  ${DATA_INSTALL_DIR}/kquerybrowser)
