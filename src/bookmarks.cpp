#include "bookmarks.h"

#include <QDebug>
#include <QStandardPaths>
#include <KBookmarkManager>

static Bookmarks instance;

void Bookmarks::init()
{
	QString path = QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/bookmarks.xml";

	qDebug() << "bookmark file" << path;
	instance.m_manager = KBookmarkManager::managerForFile(path, "kquerybrowser");
}

KBookmarkManager *Bookmarks::manager()
{
	return instance.m_manager;
}

