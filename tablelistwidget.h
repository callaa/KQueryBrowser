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
	void runQuery(const QString& query);

public slots:
	void refreshTree(const Database& db);

protected slots:
	void customContextMenu(const QPoint& point);

private:
	QTreeWidget *m_view;
};

#endif // TABLELISTWIDGET_H
