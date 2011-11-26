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
