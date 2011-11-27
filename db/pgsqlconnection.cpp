#include <QSqlQuery>
#include <QVariant>

#include "pgsqlconnection.h"
#include "../table.h"

PgsqlConnection::PgsqlConnection(QObject *parent) :
    ServerConnection(parent)
{
}

QVector<Schema> PgsqlConnection::schemas()
{
	QSqlQuery q("SELECT table_schema, table_name, column_name FROM information_schema.columns ORDER BY table_schema, table_name ASC", m_db);
	QVector<Schema> schemas;
	while(q.next()) {
		QString sname = q.value(0).toString();
		QString tname = q.value(1).toString();
		QString cname = q.value(2).toString();

		// Find schema
		Schema *schema=0;
		if(schemas.isEmpty() || schemas.last().name() != sname)
			schemas.append(Schema(sname, QVector<Table>()));
		schema = &schemas.last();

		// Find table
		Table *table=0;
		if(schema->tables().isEmpty() || schema->tables().last().name() != tname)
			schema->tables().append(Table(tname, QVector<Column>(), Table::TABLE));
		table = &schema->tables().last();

		// Add column
		table->columns().append(cname);
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
				for(int j=0;j<s.tables().count();++i) {
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

