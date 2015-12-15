cmake_minimum_required(VERSION 2.8.12)

project (kquerybrowser)
find_package(ECM REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(ECMInstallIcons)
include(ECMAddAppIcon) 

include(KDEInstallDirs)
include(KDECompilerSettings)
include(KDECMakeSettings)
include(FeatureSummary)

add_subdirectory( src )

# TODO use new desktop file naming scheme
install(PROGRAMS
	kquerybrowser.desktop
	DESTINATION ${XDG_APPS_INSTALL_DIR}
)

# TODO appdata
#install(FILES
	#${CMAKE_CURRENT_SOURCE_DIR}/data/kquerybrowser.appdata.xml
	#DESTINATION ${CMAKE_INSTALL_METAINFODIR}
#)

ecm_install_icons(ICONS 
	"hi128-app-kquerybrowser.png"
	DESTINATION ${ICON_INSTALL_DIR}
	THEME hicolor
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)

