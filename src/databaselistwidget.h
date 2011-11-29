#ifndef DATABASELISTWIDGET_H
#define DATABASELISTWIDGET_H

#include <QDockWidget>

class QListWidget;
class QStringList;

class DatabaseListWidget : public QDockWidget
{
    Q_OBJECT
public:
	explicit DatabaseListWidget(const QString& current, QWidget *parent = 0);

signals:
	void refresh();

public slots:
	void refreshList(const QStringList& databases);

protected slots:
	void customContextMenu(const QPoint& point);

private:
	QListWidget *m_view;
	QString m_current;
};

#endif // DATABASELISTWIDGET_H
