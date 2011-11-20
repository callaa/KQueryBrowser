#ifndef DATABASE_H
#define DATABASE_H

#include <QMetaType>
#include "table.h"

/**
  \brief Description of a database
  */
class Database
{
public:
    Database();
	Database(const QVector<Table> tables);

	QVector<Table> tables() const { return m_tables; }
private:
	QVector<Table> m_tables;
};

Q_DECLARE_METATYPE(Database)

#endif // DATABASE_H
