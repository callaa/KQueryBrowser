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
#include "database.h"

namespace meta {

Database::Database()
{
}

Database::Database(const QVector<Schema> schemas)
	: m_schemas(schemas)
{
}

bool Database::noSchemas() const
{
	return m_schemas.count()==1 && m_schemas.at(0).name().isEmpty();
}

}

