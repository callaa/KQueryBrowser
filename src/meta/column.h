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

namespace meta {

/**
 * \brief Column foreign key constraint
 */
class ForeignKey
{
public:
	//! Foreign key update rules
	enum Rule {
		//! Cascade changes
		CASCADE,
		//! Set column to null
		SETNULL,
		//! Set column to its default value
		SETDEFAULT,
		//! Prevent changes
		RESTRICT,
		//! Do nothing
		NOACTION,
		//! Information not available (shouldn't be encountered in normal operation)
		UNKNOWN
	};

	/**
	 * \brief Construct a blank foreign key
	 */
	ForeignKey() : m_onupdate(UNKNOWN), m_ondelete(UNKNOWN) { }

	/**
	 * \brief Construct a foreign key
	 * \param db the database of the referred column
	 * \param schema the schema of the referred column
	 * \param table the table of the referred column
	 * \param column the name of the referred column
	 * \param onupdate the ON UPDATE rule
	 * \param ondelete the ON DELETE rule
	 */
	ForeignKey(const QString& db, const QString& schema, const QString& table, const QString column, Rule onupdate, Rule ondelete)
		: m_database(db), m_schema(schema), m_table(table), m_column(column), m_onupdate(onupdate), m_ondelete(ondelete)
	{
	}

	/**
	 * \brief
	 * \return rule enum or UNKNOWN if unrecognized
	 */
	static Rule rulestring(const QString& rule);

	/**
	 * \brief Is this a valid foreign key entry?
	 * \return true if key descriptor is valid
	 */
	bool isValid() const { return !m_database.isEmpty(); }

	/**
	 * \brief The database this key refers to
	 * \return database name
	 */
	const QString& database() const { return m_database; }

	/**
	 * \brief The schema this key refers to.
	 *
	 * This is an empty string if the DBMS doesn't support schemas.
	 * \return schema name
	 */
	const QString& schema() const { return m_schema; }

	/**
	 * \brief The table this key refers to
	 * \return table name
	 */
	const QString& table() const { return m_table; }

	/**
	 * \brief The column this key refers to
	 * \return column name
	 */
	const QString& column() const { return m_column; }

	/**
	 * \brief The update rule
	 * \return update rule
	 */
	Rule onUpdate() const { return m_onupdate; }

	/**
	 * \brief the delete rule
	 * \return delete rule
	 */
	Rule onDelete() const { return m_ondelete; }

	/**
	 * \brief Get a string representation of the foreign key
	 * \return foreign key as a human readable string
	 */
	QString toString() const;

private:
	QString m_database;
	QString m_schema;
	QString m_table;
	QString m_column;
	Rule m_onupdate;
	Rule m_ondelete;
};

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

	//! Does this column have the UNIQUE constraint?
	bool isUnique() const { return m_unique; }

	//! Does this column have a foreign key constraint?
	bool hasForeignKey() const { return m_fk.isValid(); }

	/**
	 * \brief Get this column's foreign key.
	 *
	 * If hasForeignKey() returns false, this will return
	 * a blank key.
	 * \return foreign key description
	 */
	const ForeignKey& foreignkey() const { return m_fk; }

	/**
	  \brief Get the type of the column.

	  If this information is not available, an empty
	  string is returned.
	  \return type name or empty string if unknown
	  */
	const QString& type() const { return m_type; }

	void setType(const QString& type) { m_type = type; }

	void setPk(bool pk) { m_pk = pk; }

	void setUnique(bool unique) { m_unique = unique; }

	void setFk(const ForeignKey &fk) { m_fk = fk; }
private:
	QString m_name;

	// Extra info:
	QString m_type;
	bool m_pk;
	bool m_unique;
	ForeignKey m_fk;
};

}

#endif // COLUMN_H
