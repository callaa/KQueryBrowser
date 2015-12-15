//
// This file is part of KQueryBrowser.
//
// KQueryBrowser is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// KQueryBrowser is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with KQueryBrowser.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef DATABASE_H
#define DATABASE_H

#include "schema.h"

#include <QMetaType>

namespace meta {

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

}

Q_DECLARE_METATYPE(meta::Database)

#endif // DATABASE_H
