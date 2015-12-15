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
#include <QDebug>
#include "sqlcompletion.h"
#include "meta/database.h"

static const QString SQL_ITEMS[] = {
	"SELECT",
	"FROM",
	"AS",
	"LEFT JOIN",
	"NATURAL JOIN",
	"JOIN",
	"USING",
	"ON",
	"WHERE",
	"AND",
	"OR",
	"NOT",
	"ORDER BY",
	"ASC",
	"DESC"
};

QString SqlCompletion::makeCompletion( const QString& string )
{
	// TODO change completion set depending on context.
	int space = string.lastIndexOf(' ');
	QString token;
	if(space < 0)
		token = string;
	else
		token = string.mid(space+1);

	QString match = KCompletion::makeCompletion(token);
	qDebug() << "token:" << token << ", match:" << match;
	if(space < 0) {
		return match;
	} else {
		if(match.isEmpty())
			return match;
		return string.left(space+1) + match;
	}
}

void SqlCompletion::refreshModel(const meta::Database& db)
{
	clear();
	for(unsigned int i=0;i<sizeof(SQL_ITEMS)/sizeof(QString);++i)
		addItem(SQL_ITEMS[i]);

	for(const meta::Schema &s : db.schemas()) {
		addItem(s.name());
		for(const meta::Table &t : s.tables()) {
			addItem(t.name());
			for(const meta::Column &c : t.columns()) {
				addItem(c.name());
			}
		}
	}
}

