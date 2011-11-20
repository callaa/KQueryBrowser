#ifndef QUERYJSOBJECT_H
#define QUERYJSOBJECT_H

#include <QObject>

/**
  \brief An object that can be used via the query browser JavaScript
  */
class QueryJsObject : public QObject
{
    Q_OBJECT
public:
	explicit QueryJsObject(QueryWidget *parent);

signals:

public slots:
	void performQuery(const QString& query);

private:
	QueryWidget *m_qw;
};

#endif // QUERYJSOBJECT_H
