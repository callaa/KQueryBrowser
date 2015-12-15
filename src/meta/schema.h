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
#ifndef SCHEMA_H
#define SCHEMA_H

#include "table.h"

namespace meta {

class Schema
{
public:
	Schema();
	Schema(const QString& name, const QVector<Table> tables);

	const QString& name() const { return m_name; }
	const QVector<Table>& tables() const { return m_tables; }
	QVector<Table>& tables() { return m_tables; }

private:
	QString m_name;
	QVector<Table> m_tables;
};

}

#endif

