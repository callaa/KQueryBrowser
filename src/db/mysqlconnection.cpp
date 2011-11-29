//
// This file is part of KQueryBrowser.
//
// KQueryBrowser is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KQueryBrowser is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KQueryBrowser.  If not, see <http://www.gnu.org/licenses/>.
//
#include <QSqlQuery>
#include <QVariant>
#include <QStringList>

#include "mysqlconnection.h"
#include "../meta/table.h"

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

QStringList MysqlConnection::databases()
{
	QSqlQuery q("SHOW DATABASES", m_db);
	QStringList list;
	while(q.next())
		list << q.value(0).toString();
	return list;
}
