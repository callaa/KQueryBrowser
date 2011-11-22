#ifndef DBCTXMANAGER_H
#define DBCTXMANAGER_H

#include <QObject>
#include <QVector>

#include "table.h"

class Connection;
class Database;

class DbCtxManager : public QObject
{
    Q_OBJECT
public:
	explicit DbCtxManager(Connection *connection);

signals:
	void dbStructure(const Database& db);

public slots:
	void createContext(QObject *forthis);
	void removeContext(QObject *forthis);
	void getDbStructure();

private:
	void addTables(QVector<Table> &tables, const QStringList& names, Table::Type type);
	Connection *m_connection;

};

#endif // DBCTXMANAGER_H
