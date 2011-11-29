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

PgsqlConnection::PgsqlConnection(QObject *parent) :
    ServerConnection(parent)
{
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
	QSqlQuery q("SELECT table_schema, table_name, column_name, data_type,character_maximum_length FROM information_schema.columns ORDER BY table_schema, table_name ASC", m_db);
	QVector<Schema> schemas;
	while(q.next()) {
		QString sname = q.value(0).toString();
		QString tname = q.value(1).toString();
		QString cname = q.value(2).toString();
		QString ctype = q.value(3).toString();
		QVariant cmaxlen = q.value(4);

		// Find schema
		Schema *schema=0;
		if(schemas.isEmpty() || schemas.last().name() != sname)
			schemas.append(Schema(sname, QVector<Table>()));
		schema = &schemas.last();

		// Find table (TODO identify type [normal, view, system])
		Table *table=0;
		if(schema->tables().isEmpty() || schema->tables().last().name() != tname)
			schema->tables().append(Table(tname, QVector<Column>(), Table::TABLE));
		table = &schema->tables().last();

		// Add column
		table->columns().append(cname);
		table->columns().last().setType(typestr(ctype, cmaxlen));
	}

	// Get keys (TODO identify type)
	q.exec("SELECT table_schema, table_name, column_name FROM information_schema.key_column_usage");
	while(q.next()) {
		QString schema = q.value(0).toString();
		QString table = q.value(1).toString();
		QString column = q.value(2).toString();

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
								c.setPk(true);
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
