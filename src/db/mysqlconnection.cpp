#include <QDebug>
#include <QSqlQuery>
#include <QVariant>

#include "mysqlconnection.h"
#include "../table.h"

MysqlConnection::MysqlConnection(QObject *parent) :
    ServerConnection(parent)
{
}

QVector<Schema> MysqlConnection::schemas()
{
	QVector<Table> tables;

	// Get tables
	QSqlQuery q("show full tables", m_db);
	while(q.next()) {
		tables.append(Table(q.value(0).toString(), QVector<Column>(), q.value(1).toString() == "VIEW" ? Table::VIEW : Table::TABLE));
	}

	// Get columns for tables
	for(int i=0;i<tables.count();++i) {
		Table &t = tables[i];
		q.exec("explain " + t.name());

		// Explanation columns are: Field, type, null, key (PRI), default, extra
		while(q.next()) {
			Column c(q.value(0).toString());
			c.setType(q.value(1).toString());
			c.setPk(q.value(3).toBool());
			t.columns().append(c);
		}
	}

	QVector<Schema> schemas(1);
	schemas[0] = Schema(QString(), tables);
	return schemas;
}

