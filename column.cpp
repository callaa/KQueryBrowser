#include "column.h"

Column::Column()
{
}

Column::Column(const QString &name)
	: m_name(name)
{
}


void Column::setExtraInfo(bool pk, const QString& type)
{
	m_pk = pk;
	m_type = type;
}
