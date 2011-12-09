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

static ForeignKey::Rule rule2enum(const QString& rule) {
	if(rule == "CASCADE")
		return ForeignKey::CASCADE;
	else if(rule=="SET NULL")
		return ForeignKey::SETNULL;
	else if(rule=="SET DEFAULT")
		return ForeignKey::SETDEFAULT;
	else if(rule=="RESTRICT")
		return ForeignKey::RESTRICT;
	else if(rule=="NO ACTION")
		return ForeignKey::NOACTION;
	else
		return ForeignKey::UNKNOWN;
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

		// Explanation columns are: Field, type, null, key, default, extra
		while(q.next()) {
			Column c(q.value(0).toString());
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
			Table &t = tables[i];
			if(t.name() != tname)
				continue;
			for(int j=0;j<t.columns().count();++j) {
				Column &c = t.columns()[j];
				if(c.name() != cname)
					continue;

				// Set foreign key
				c.setFk(ForeignKey(
							q.value(2).toString(),
							QString(),
							q.value(3).toString(),
							q.value(4).toString(),
							rule2enum(q.value(5).toString()),
							rule2enum(q.value(6).toString())
							));
			}
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

