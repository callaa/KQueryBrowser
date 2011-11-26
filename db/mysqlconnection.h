#ifndef MYSQLCONNECTION_H
#define MYSQLCONNECTION_H

#include "serverconnection.h"

class MysqlConnection : public ServerConnection
{
    Q_OBJECT
public:
    explicit MysqlConnection(QObject *parent = 0);

protected:
	QString type() const { return QString("QMYSQL"); }
	int defaultPort() const { return 3306; }

	void setExtraInfo(Table &table);
};

#endif // MYSQLCONNECTION_H
