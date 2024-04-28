set(kquerybrowser_SRCS
    main.cpp
    windows/mainwindow.h windows/mainwindow.cpp
    windows/newconnection.ui windows/newconnectiondialog.h windows/newconnectiondialog.cpp
    windows/bigvalue.ui windows/bigvaluedialog.h windows/bigvaluedialog.cpp
    queryview/webengine.h queryview/webengine.cpp
    queryview/queryview.h queryview/queryview.cpp
    queryview/webapi.h queryview/webapi.cpp
    db/database.h db/database.cpp
    db/queryresult.h db/queryresult.cpp
    db/sqliteadapter.h db/sqliteadapter.cpp
    db/genericqtadapter.h db/genericqtadapter.cpp
    db/qtqueryresult.h db/qtqueryresult.cpp
    db/errorresult.h db/errorresult.cpp
    db/schema.h db/schema.cpp
    widgets/sqllineedit.h widgets/sqllineedit.cpp
    widgets/tablelist.h widgets/tablelist.cpp
    widgets/dblist.h widgets/dblist.cpp
    widgets/findwidget.h widgets/findwidget.cpp
    widgets/scripteditor.h widgets/scripteditor.cpp
    utils/bookmarks.h utils/bookmarks.cpp
    utils/sqlcompletion.h utils/sqlcompletion.cpp
    utils/jsonnmodel.h utils/jsonnmodel.cpp
)

qt_add_executable(kquerybrowser
    ${kquerybrowser_SRCS}
    html/html.qrc
    res/resources.qrc
)

target_include_directories(kquerybrowser INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(kquerybrowser PRIVATE
    Qt6::Widgets
    Qt6::WebEngineWidgets
    Qt6::Sql
    KF6::CoreAddons
    KF6::I18n
    KF6::XmlGui
    KF6::KIOCore
    KF6::KIOFileWidgets
    KF6::Bookmarks
    KF6::BookmarksWidgets
    KF6::TextEditor
)
