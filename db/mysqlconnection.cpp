#include <QSqlQuery>
#include <QVariant>

#include "mysqlconnection.h"
#include "../table.h"

MysqlConnection::MysqlConnection(QObject *parent) :
    ServerConnection(parent)
{
}

void MysqlConnection::setExtraInfo(Table &table)
{
	QSqlQuery q("explain " + table.name(), m_db);

	// Explanation columns are: Field, type, null, key (PRI), default, extra
	while(q.next()) {
		Column *c = table.column(q.value(0).toString());
		if(c==0) {
			qWarning(QString("setExtraInfo: No such column: %1").arg(q.value(0).toString()).toLatin1());
			continue;
		}
		c->setExtraInfo(
					q.value(3).toString() == "PRI", // PK
					q.value(1).toString()); // type
	}
}
