#include "bookmarks.h"

#include <QDebug>
#include <QStandardPaths>
#include <KBookmarkManager>

KBookmarkManager *INSTANCE = nullptr;

KBookmarkManager *sharedBookmarkManager()
{
    if(!INSTANCE) {
        const QString path = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/bookmarks.xml");
        INSTANCE = new KBookmarkManager(path);
    }

    return INSTANCE;
}
