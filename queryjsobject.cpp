#include <QDebug>

#include "querywidget.h"
#include "queryjsobject.h"

QueryJsObject::QueryJsObject(QueryWidget *parent) :
	QObject(parent), m_qw(parent)
{
}


void QueryJsObject::performQuery(const QString &query)
{
	qDebug() << "QUERY:" << query;
}
