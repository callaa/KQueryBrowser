#include "column.h"

Column::Column()
	: m_pk(false)
{
}

Column::Column(const QString &name)
	: m_name(name), m_pk(false)
{
}

