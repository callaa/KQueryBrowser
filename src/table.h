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
#ifndef TABLE_H
#define TABLE_H

#include <QVector>

#include "column.h"

/**
  \brief A description of a table
  */
class Table
{
public:
	enum Type {TABLE, VIEW, SYSTEMTABLE};

	Table();
	Table(const QString& name, const QVector<Column> &columns, Type type);

	//! Get the name of this table
	const QString& name() const { return m_name; }

	//! Get the columns in this table
	const QVector<Column> &columns() const { return m_columns; }

	QVector<Column> &columns() { return m_columns; }

	/**
	  \brief Get the named column

	  \return pointer to column or 0 if not found
	  */
	Column *column(const QString& name);

	//! Get the table type
	Type type() const { return m_type; }

private:
	QString m_name;
	QVector<Column> m_columns;
	Type m_type;
};

#endif // TABLE_H
