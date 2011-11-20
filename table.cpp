#include "table.h"


Table::Table()
{

}

Table::Table(const QString& name, const QVector<Column> &columns, bool view)
	: m_name(name), m_columns(columns), m_view(view)
{

}
