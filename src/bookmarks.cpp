#include <QDebug>
#include <KBookmarkManager>
#include <KStandardDirs>

#include "bookmarks.h"

static Bookmarks instance;

void Bookmarks::init()
{
	qDebug() << "bookmark file" << KStandardDirs::locateLocal("appdata", "bookmarks.xml");
	instance.m_manager = KBookmarkManager::managerForFile(
			KStandardDirs::locateLocal("appdata", "bookmarks.xml"),
			"kquerybrowser"
			);
}

KBookmarkManager *Bookmarks::manager()
{
	return instance.m_manager;
}

