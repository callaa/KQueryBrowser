#ifndef PGSQLCONNECTION_H
#define PGSQLCONNECTION_H

#include "serverconnection.h"

class PgsqlConnection : public ServerConnection
{
    Q_OBJECT
public:
    explicit PgsqlConnection(QObject *parent = 0);

protected:
	QString type() const { return QString("QPSQL"); }
	int defaultPort() const { return 5432; }

	void setExtraInfo(Table &table);
};

#endif // PGSQLCONNECTION_H
