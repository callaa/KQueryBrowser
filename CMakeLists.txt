project (kquerybrowser)
find_package(KDE4 REQUIRED)
include (KDE4Defaults)
include_directories(${KDE4_INCLUDES})

set(kquerybrowser_SRCS
	main.cpp
	mainwindow.cpp
	connectiondialog.cpp
	connection.cpp
	sqlite3connection.cpp
	querywidget.cpp
	queryjsobject.cpp
	dbctxmanager.cpp
	dbcontext.cpp
	queryresults.cpp
	database.cpp
	table.cpp
	column.cpp
	tablelistwidget.cpp
)

kde4_add_ui_files( kquerybrowser_SRCS connectiondialog.ui )

kde4_add_executable(kquerybrowser ${kquerybrowser_SRCS})

target_link_libraries(kquerybrowser ${KDE4_KDEUI_LIBS} ${KDE4_KIO_LIBS} ${QT_QTSQL_LIBRARY} ${QT_QTWEBKIT_LIBRARY})
install(TARGETS kquerybrowser  ${INSTALL_TARGETS_DEFAULT_ARGS})
install(FILES kquerybrowserui.rc
		DESTINATION  ${DATA_INSTALL_DIR}/kquerybrowser)
