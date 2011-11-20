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
	Table();
	Table(const QString& name, const QVector<Column> &columns, bool view);

	const QString& name() const { return m_name; }
	const QVector<Column> columns() const { return m_columns; }
	bool isView() const { return m_view; }

private:
	QString m_name;
	QVector<Column> m_columns;
	bool m_view;
};

#endif // TABLE_H
