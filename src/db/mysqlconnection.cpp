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
#include <QSqlError>
#include <QVariant>
#include <QStringList>

#include "mysqlconnection.h"
#include "../meta/table.h"

MysqlConnection::MysqlConnection(const KUrl& url, QObject *parent) :
    ServerConnection(url, parent)
{
}

bool MysqlConnection::isCapable(Capability capability) const
{
	switch(capability) {
		case SHOW_CREATE: return true;
		case SWITCH_DB: return true;
		default: return false;
	}
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

bool MysqlConnection::selectDatabase(const QString& database)
{
	QSqlQuery q(m_db);
	if(q.exec("USE " + database)) {
		KUrl newurl = url();
		newurl.setPath(database);
		changeUrl(newurl);
		return true;
	}
	return false;
}

QString MysqlConnection::createScript(const QString& table)
{
	QSqlQuery q("SHOW CREATE TABLE " + table, m_db);
	// This query returns two columns: table name, create
	if(!q.next()) {
		qWarning("Couldn't show table (%s) creation script: %s",
				table.toAscii().constData(),
				q.lastError().text().toAscii().constData());
		return QString();
	}
	return q.value(1).toString();
}

