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
#include "table.h"


Table::Table()
{

}

Table::Table(const QString& name, const QVector<Column> &columns, Type type)
	: m_name(name), m_columns(columns), m_type(type)
{

}

Column *Table::column(const QString &name)
{
	for(int i=0;i<m_columns.size();++i)
		if(m_columns[i].name() == name)
			return &m_columns[i];
	return 0;
}
