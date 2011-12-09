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
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QStringList>

#include "sqlite3connection.h"
#include "../meta/table.h"

Sqlite3Connection::Sqlite3Connection(const KUrl& url, QObject *parent) :
	Connection(url, parent)
{
}

bool Sqlite3Connection::isCapable(Capability capability) const
{
	switch(capability) {
		case SHOW_CREATE: return true;
		default: return false;
	}
}

void Sqlite3Connection::prepareConnection(QSqlDatabase &db)
{
	db.setDatabaseName(url().path());
}

QString Sqlite3Connection::name() const
{
	return url().fileName();
}

QVector<Schema> Sqlite3Connection::schemas()
{
	QVector<Table> tables;

	QSqlQuery q("SELECT type, name FROM sqlite_master WHERE type IN ('table', 'view') ORDER BY type, name ASC", m_db);
	while(q.next()) {
		tables.append(Table(q.value(1).toString(), QVector<Column>(), q.value(0).toString() == "table" ? Table::TABLE : Table::VIEW));
	}
	// Add master table to the list too
	tables.append(Table("sqlite_master", QVector<Column>(), Table::SYSTEMTABLE));

	// Get column definitions
	for(int i=0;i<tables.count();++i) {
		Table &t = tables[i];

		q.exec("pragma table_info(" + t.name() + ")");

		// Pragma columns are: column id (zero based), column name, column type, not-null, default value, primary key
		while(q.next()) {
			Column c = Column(q.value(1).toString());
			c.setType(q.value(2).toString());
			c.setPk(q.value(5).toBool());
			t.columns().append(c);
		}

		// Find unique indexes
		// Index list returns three columns: seq, name, unique
		q.exec("pragma index_list(" + t.name() + ")");
		while(q.next()) {
			if(q.value(2).toBool()) {
				// index_info returns three columns: seqno, cid, name
				QSqlQuery iq("pragma index_info(" + q.value(1).toString() + ")", m_db);
				while(iq.next()) {
					t.column(iq.value(2).toString())->setUnique(true);
				}
			}
		}

		// TODO get foreign keys
	}

	QVector<Schema> schemas(1);
	schemas[0] = Schema(QString(), tables);
	return schemas;
}

QStringList Sqlite3Connection::databases()
{
	// TODO return list of attached databases
	QStringList list;
	list << name();
	return list;
}

QString Sqlite3Connection::createScript(const QString& table)
{
	QSqlQuery q(m_db);
	q.prepare("SELECT sql FROM sqlite_master WHERE tbl_name=?");
	q.bindValue(0, table);
	q.exec();
	if(q.next()==false) {
		qWarning("Couldn't show table (%s) creation script: %s",
				table.toAscii().constData(),
				q.lastError().text().toAscii().constData());
		return QString();
	}
	return q.value(0).toString();
}

