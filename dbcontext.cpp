#include <QDebug>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlError>

#include <climits>

#include "dbcontext.h"
#include "queryresults.h"

DbContext::DbContext(QSqlDatabase &db, QObject *parent) :
	QObject(parent), m_db(db)
{
}

void DbContext::doQuery(const QString &querystr, int limit)
{
	QSqlQuery q(m_db);
	q.setForwardOnly(true);
	QueryResultsData *res = new QueryResultsData();

	// Note. This is technically different than no limit at all, but
	// practically, nobody is going to read INT_MAX rows anyway.
	if(limit<=0)
		limit = INT_MAX;

	if(q.exec(querystr)) {
		res->success = true;
		res->more = false;
		if(q.isSelect()) {
			res->select = true;
			res->rowcount = q.size();
			const int cols = q.record().count();
			res->columns.resize(cols);
			for(int i=0;i<cols;++i)
				res->columns[i] = Column(q.record().fieldName(i));

			int count=0;
			while(q.next()) {
				ResultRow row(cols);
				for(int i=0;i<cols;++i)
					row[i] = q.value(i);
				res->rows.append(row);
				if(++count>=limit) {
					res->more = true;
					break;
				}
			}

		} else {
			res->select = false;
			res->rowcount = q.numRowsAffected();
		}
	} else {
		res->success = false;
		res->more = false;
		res->select = q.isSelect();
		res->rowcount = 0;
		res->error = m_db.lastError().text();
	}

	emit results(QueryResults(res));
}
