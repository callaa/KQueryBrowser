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
#include <QDebug>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>
#include <QDebug>

#include "mysqlconnection.h"
#include "../../meta/database.h"
#include "../../meta/table.h"
#include "../../meta/schema.h"

namespace db {

MysqlConnection::MysqlConnection(const QUrl& url, QObject *parent) :
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

void MysqlConnection::doGetDbStructure()
{
	QVector<meta::Table> tables;

	// Get tables
	QSqlQuery q("show full tables", m_db);
	while(q.next()) {
		tables.append(meta::Table(q.value(0).toString(), QVector<meta::Column>(), q.value(1).toString() == "VIEW" ? meta::Table::VIEW : meta::Table::TABLE));
	}

	// Get columns for tables
	for(int i=0;i<tables.count();++i) {
		meta::Table &t = tables[i];
		q.exec("explain " + t.name());

		// Explanation columns are: Field, type, null, key, default, extra
		while(q.next()) {
			meta::Column c(q.value(0).toString());
			c.setType(q.value(1).toString());
			c.setPk(q.value(3).toString() == "PRI");
			c.setUnique(q.value(3).toString() == "UNI");
			t.columns().append(c);
		}
	}

	// Get foreign keys
	q.prepare("SELECT kcu.table_name, kcu.column_name, kcu.referenced_table_schema, "
			"kcu.referenced_table_name, kcu.referenced_column_name, rc.update_rule, "
			"rc.delete_rule "
			"FROM information_schema.key_column_usage kcu JOIN "
			"information_schema.referential_constraints rc "
			"USING (constraint_schema, constraint_name) "
			"WHERE kcu.constraint_schema=?");
	q.bindValue(0, name());
	q.exec();
	while(q.next()) {
		QString tname = q.value(0).toString(); // Name of the referring table
		QString cname = q.value(1).toString(); // Name of the referring column

		// Find table and column
		for(int i=0;i<tables.count();++i) {
			meta::Table &t = tables[i];
			if(t.name() != tname)
				continue;
			for(int j=0;j<t.columns().count();++j) {
				meta::Column &c = t.columns()[j];
				if(c.name() != cname)
					continue;

				// Set foreign key
				c.setFk(meta::ForeignKey(
							q.value(2).toString(),
							QString(),
							q.value(3).toString(),
							q.value(4).toString(),
							meta::ForeignKey::rulestring(q.value(5).toString()),
							meta::ForeignKey::rulestring(q.value(6).toString())
							));
			}
		}
	}

	QVector<meta::Schema> schemas(1);
	schemas[0] = meta::Schema(QString(), tables);
	emit dbStructure(meta::Database(schemas));
}

void MysqlConnection::doGetDbList()
{
	QSqlQuery q("SHOW DATABASES", m_db);
	QStringList list;
	while(q.next())
		list << q.value(0).toString();
	emit dbList(list, name());
}

void MysqlConnection::doSwitchDatabase(const QString& database)
{
	QSqlQuery q(m_db);
	if(q.exec("USE " + database)) {
		QUrl newurl = url();
		newurl.setPath(database);
		changeUrl(newurl);
		doGetDbList();
		doGetDbStructure();
	}
}

void MysqlConnection::doGetCreateScript(const QString& table)
{
	QSqlQuery q("SHOW CREATE TABLE " + table, m_db);
	// This query returns two columns: table name, create
	if(!q.next()) {
		qWarning() << "Couldn't show table (" << table << ") creation script: " << q.lastError().text();
		emit newScript(q.lastError().text());
	}

	emit newScript(q.value(1).toString());
}

}

