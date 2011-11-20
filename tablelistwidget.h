#ifndef TABLELISTWIDGET_H
#define TABLELISTWIDGET_H

#include <QDockWidget>

class QTreeWidget;
class Database;

class TableListWidget : public QDockWidget
{
    Q_OBJECT
public:
    explicit TableListWidget(QWidget *parent = 0);

signals:

public slots:
	void refreshTree(const Database& db);

private:
	QTreeWidget *m_view;
};

#endif // TABLELISTWIDGET_H
