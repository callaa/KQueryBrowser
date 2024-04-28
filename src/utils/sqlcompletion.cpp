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
#include "sqlcompletion.h"
#include "db/schema.h"

#include <QDebug>

using namespace Qt::StringLiterals;

static const QString SQL_ITEMS[] = {
    u"SELECT"_s,
    u"FROM"_s,
    u"AS"_s,
    u"LEFT JOIN"_s,
    u"NATURAL JOIN"_s,
    u"JOIN"_s,
    u"USING"_s,
    u"ON"_s,
    u"WHERE"_s,
    u"AND"_s,
    u"OR"_s,
    u"NOT"_s,
    u"ORDER BY"_s,
    u"ASC"_s,
    u"DESC"_s
};

QString SqlCompletion::makeCompletion(const QString& string)
{
	// TODO change completion set depending on context.
    int space = string.lastIndexOf(u' ');
	QString token;
	if(space < 0)
		token = string;
	else
		token = string.mid(space+1);

	QString match = KCompletion::makeCompletion(token);

	if(space < 0) {
		return match;
	} else {
		if(match.isEmpty())
			return match;
		return string.left(space+1) + match;
	}
}

void SqlCompletion::refreshModel(const dbschema::Database& db)
{
	clear();
	for(unsigned int i=0;i<sizeof(SQL_ITEMS)/sizeof(QString);++i)
		addItem(SQL_ITEMS[i]);

    for(const auto &s : db.schemas) {
        if(!s.name.isEmpty())
            addItem(s.name);
        for(const auto &t : s.tables) {
            addItem(t.name);
            for(const auto &c : t.columns) {
                addItem(c.name);
			}
		}
	}
}

