#include "database.h"

Database::Database()
{
}

Database::Database(const QVector<Table> tables)
	: m_tables(tables)
{
}
