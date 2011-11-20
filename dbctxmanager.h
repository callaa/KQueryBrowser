#ifndef DBCTXMANAGER_H
#define DBCTXMANAGER_H

#include <QObject>

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
	void createContext(const QObject *forthis);
	void getDbStructure();

private:
	Connection *m_connection;
};

#endif // DBCTXMANAGER_H
