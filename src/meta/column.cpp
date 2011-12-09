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
#include "column.h"
#include "../stringbuilder.h"

ForeignKey::Rule ForeignKey::rulestring(const QString& rule)
{
	if(rule == "CASCADE")
		return ForeignKey::CASCADE;
	else if(rule=="SET NULL")
		return ForeignKey::SETNULL;
	else if(rule=="SET DEFAULT")
		return ForeignKey::SETDEFAULT;
	else if(rule=="RESTRICT")
		return ForeignKey::RESTRICT;
	else if(rule=="NO ACTION")
		return ForeignKey::NOACTION;
	else
		return ForeignKey::UNKNOWN;
}

Column::Column()
	: m_pk(false), m_unique(false)
{
}

Column::Column(const QString &name)
	: m_name(name), m_pk(false), m_unique(false)
{
}

// Note. Make sure these are in the same order as in the enum
static const QString FK_RULES[] = {
	"CASCADE",
	"SET NULL",
	"SET DEFAULT",
	"RESTRICT",
	"NO ACTION",
	"UNKNOWN"
};

QString ForeignKey::toString() const
{
	if(!isValid())
		return QString();

	StringBuilder fk;
	const QString DOT = ".";
	fk << m_database << DOT;
	if(!m_schema.isEmpty())
		fk << m_schema << DOT;
	fk << m_table <<  DOT << m_column;

	fk << " ON UPDATE " << FK_RULES[m_onupdate];
	fk << " ON DELETE " << FK_RULES[m_ondelete];
	return fk.toString();
}

