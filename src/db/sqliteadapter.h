#ifndef SQLITEADAPTER_H
#define SQLITEADAPTER_H

#include "genericqtadapter.h"

class SqliteAdapter : public GenericQtAdapter
{
public:
    SqliteAdapter(const QUrl &url, QObject *parent);
    void refreshSchema() override;
};

#endif // SQLITEADAPTER_H
