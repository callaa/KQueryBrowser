#ifndef SCHEMA_H
#define SCHEMA_H

#include "table.h"

class Schema
{
public:
	Schema();
	Schema(const QString& name, const QVector<Table> tables);

	const QString& name() const { return m_name; }
	const QVector<Table>& tables() const { return m_tables; }
	QVector<Table>& tables() { return m_tables; }

private:
	QString m_name;
	QVector<Table> m_tables;
};

#endif

