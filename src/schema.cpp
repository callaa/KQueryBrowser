#include "schema.h"

Schema::Schema()
{
}

Schema::Schema(const QString& name, const QVector<Table> tables)
	: m_name(name), m_tables(tables)
{
}

