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
