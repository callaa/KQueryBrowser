#include "table.h"


Table::Table()
{

}

Table::Table(const QString& name, const QVector<Column> &columns, Type type)
	: m_name(name), m_columns(columns), m_type(type)
{

}
