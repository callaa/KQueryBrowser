#ifndef GENERICQTADAPTER_H
#define GENERICQTADAPTER_H

#include "database.h"

#include <QSqlDatabase>

//! Database connection adapter for generic QtSQL databases
class GenericQtAdapter : public DatabaseAdapter
{
public:
    GenericQtAdapter(const QString &dbDriver, const QUrl &url, QObject *parent);
    ~GenericQtAdapter();

    bool isOpen() const override;
    QString name() const override;
    QString databaseError() const override;

    QueryResult *query(const QString &query, int maxRows) override;
    void refreshSchema() override;
    void refreshOtherDatabases() override;

protected:
    QSqlDatabase m_db;
};

#endif // GENERICQTADAPTER_H
