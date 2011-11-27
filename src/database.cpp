#include "database.h"

Database::Database()
{
}

Database::Database(const QVector<Schema> schemas)
	: m_schemas(schemas)
{
}

bool Database::noSchemas() const
{
	return m_schemas.count()==1 && m_schemas.at(0).name().isEmpty();
}

