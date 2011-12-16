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

#include "pgsqlconnection.h"
#include "../meta/table.h"
#include "../stringbuilder.h"

PgsqlConnection::PgsqlConnection(const KUrl& url, QObject *parent) :
    ServerConnection(url, parent)
{
}

bool PgsqlConnection::isCapable(Capability capability) const
{
	switch(capability) {
		default: return false;
	}
}

static QString typestr(const QString &datatype, const QVariant &maxcharlen)
{
	// TODO add more type info (numeric precision etc.)
	StringBuilder type;
	type << datatype;
	if(!maxcharlen.isNull())
		type << QString(" (%1)").arg(maxcharlen.toInt());
	return type.toString();
}

QVector<Schema> PgsqlConnection::schemas()
{
	QSqlQuery q("SELECT table_schema, table_name, table_type, column_name, data_type,character_maximum_length FROM information_schema.columns NATURAL JOIN information_schema.tables ORDER BY table_schema, table_name, ordinal_position ASC", m_db);
	QVector<Schema> schemas;
	while(q.next()) {
		QString sname = q.value(0).toString();
		QString tname = q.value(1).toString();
		QString ttype = q.value(2).toString();
		QString cname = q.value(3).toString();
		QString ctype = q.value(4).toString();
		QVariant cmaxlen = q.value(5);

		// Find schema
		Schema *schema=0;
		if(schemas.isEmpty() || schemas.last().name() != sname)
			schemas.append(Schema(sname, QVector<Table>()));
		schema = &schemas.last();

		// Find table
		Table *table=0;
		if(schema->tables().isEmpty() || schema->tables().last().name() != tname) {
			Table::Type type = Table::TABLE;
			if(ttype=="VIEW")
				type = Table::VIEW;
			else if(sname == "pg_catalog")
				type = Table::SYSTEMTABLE;
			schema->tables().append(Table(tname, QVector<Column>(), type));
		}
		table = &schema->tables().last();

		// Add column
		table->columns().append(cname);
		table->columns().last().setType(typestr(ctype, cmaxlen));
	}

	// Get constraints
	q.exec("SELECT"
			" table_schema, table_name, column_name, constraint_type, NULL, NULL, NULL, NULL, NULL, NULL"
			" FROM information_schema.table_constraints"
			" NATURAL JOIN information_schema.constraint_column_usage"
			" WHERE constraint_type!='FOREIGN KEY'"

			" UNION "

			"SELECT"
			" c.table_schema, c.table_name, c.column_name, 'FOREIGN KEY', rk.table_catalog, rk.table_schema, rk.table_name, rk.column_name, update_rule, delete_rule"
			" FROM information_schema.referential_constraints"
			" NATURAL JOIN information_schema.key_column_usage c"
			" JOIN information_schema.key_column_usage rk ON (unique_constraint_schema=rk.constraint_schema AND unique_constraint_name=rk.constraint_name)"

			" ORDER BY table_schema, table_name, column_name ASC"
		);
	while(q.next()) {
		QString schema = q.value(0).toString();
		QString table = q.value(1).toString();
		QString column = q.value(2).toString();
		QString type = q.value(3).toString();

		// TODO optimize
		for(int i=0;i<schemas.count();++i) {
			Schema &s = schemas[i];
			if(s.name() == schema) {
				for(int j=0;j<s.tables().count();++j) {
					Table &t = s.tables()[j];
					if(t.name() == table) {
						for(int k=0;k<t.columns().count();++k) {
							Column &c = t.columns()[k];
							if(c.name() == column) {
								if(type == "FOREIGN KEY") {
									c.setFk(ForeignKey(
												q.value(4).toString(),
												q.value(5).toString(),
												q.value(6).toString(),
												q.value(7).toString(),
												ForeignKey::rulestring(q.value(8).toString()),
												ForeignKey::rulestring(q.value(9).toString())
												));
								} else if(type=="PRIMARY KEY") {
									c.setPk(true);
								} else if(type=="UNIQUE") {
									c.setUnique(true);
								}
							}
						}
						break;
					}
				}
				break;
			}
		}
	}

	return schemas;
}

QStringList PgsqlConnection::databases()
{
	QStringList list;
	QSqlQuery q("SELECT datname FROM pg_database WHERE datallowconn=true ORDER BY datname ASC", m_db);
	while(q.next())
		list << q.value(0).toString();
	return list;
}

