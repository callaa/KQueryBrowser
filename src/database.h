#ifndef DATABASE_H
#define DATABASE_H

#include <QMetaType>
#include "schema.h"

/**
  \brief Description of a database

  A database contains (usually) one or more schemas, which contain
  the tables.
  For database types that do not support schemas, this object will
  contain a single unnamed schema.
  */
class Database
{
public:
    Database();
	Database(const QVector<Schema> schemas);

	QVector<Schema> schemas() const { return m_schemas; }

	/**
	  \brief True if this database has doesn't support schemas.

	  If this function returns true, schemas() contains just
	  a single unnamed schema.
	  */
	bool noSchemas() const;
private:
	QVector<Schema> m_schemas;
};

Q_DECLARE_METATYPE(Database)

#endif // DATABASE_H
