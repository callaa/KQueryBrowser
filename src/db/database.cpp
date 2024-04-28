#include "database.h"

#include "sqliteadapter.h"
#include "genericqtadapter.h"

#include <QUrl>

using namespace Qt::StringLiterals;

DatabaseAdapter::DatabaseAdapter(const QUrl &url, QObject *parent)
    : QObject(parent), m_url(url), m_nextQueryId(0)
{

}

QueryId DatabaseAdapter::nextQueryId() {
    return ++m_nextQueryId;
}

DatabaseAdapter *DatabaseAdapter::openConnection(const QUrl &url, QObject *parent) {
    if(url.scheme() == u"sqlite"_s || url.scheme() == u"sqlite3"_s) {
        return new SqliteAdapter(url, parent);
    } else if(url.scheme() == u"mariadb"_s || url.scheme() == u"mysql"_s) {
        return new GenericQtAdapter(u"QMYSQL"_s, url, parent);
    } else if(url.scheme() == u"postgres"_s || url.scheme() == u"postgresql"_s) {
        return new GenericQtAdapter(u"QPSQL"_s, url, parent);
    } else {
        qWarning() << "Unknown database type:" << url.scheme();
        return nullptr;
    }
}
