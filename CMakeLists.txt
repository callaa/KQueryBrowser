cmake_minimum_required(VERSION 3.20)

set(QT_MIN_VERSION "6.7.0")
set(KF_MIN_VERSION "6.0.0")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTORCC ON)

project(kquerybrowser VERSION 1.0.0 LANGUAGES CXX)

find_package(ECM ${KF_MIN_VERSION} REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(FeatureSummary)

find_package(Qt6 ${QT_MIN_VERSION} REQUIRED COMPONENTS Widgets WebEngineWidgets Sql)
qt_standard_project_setup()

find_package(KF6 ${KF_MIN_VERSION} REQUIRED COMPONENTS
    CoreAddons
    I18n
    XmlGui
    TextWidgets
    KIO
    Bookmarks
    TextEditor
)

add_subdirectory(src)
