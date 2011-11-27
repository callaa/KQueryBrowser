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
#ifndef COLUMN_H
#define COLUMN_H

#include <QString>

/**
  \brief Table column description
  */
class Column
{
public:
	Column();
	Column(const QString& name);

	//! Get the name of the column
	const QString& name() const { return m_name; }

	//! Is this column a (part of a) primary key?
	bool isPrimaryKey() const { return m_pk; }

	//! Is this column a foreign key? TODO
	bool isForeignKey() const { return false; }

	/**
	  \brief Get the type of the column.

	  If this information is not available, an empty
	  string is returned.
	  \return type name or empty string if unknown
	  */
	const QString& type() const { return m_type; }

	void setType(const QString& type) { m_type = type; }

	void setPk(bool pk) { m_pk = pk; }

private:
	QString m_name;

	// Extra info:
	QString m_type;
	bool m_pk;
};

#endif // COLUMN_H
