#include "table.h"


Table::Table()
{

}

Table::Table(const QString& name, const QVector<Column> &columns, Type type)
	: m_name(name), m_columns(columns), m_type(type)
{

}

Column *Table::column(const QString &name)
{
	for(int i=0;i<m_columns.size();++i)
		if(m_columns[i].name() == name)
			return &m_columns[i];
	return 0;
}
