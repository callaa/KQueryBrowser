#ifndef DBLIST_H
#define DBLIST_H

#include <QDockWidget>

class QListWidget;

class DbListDock : public QDockWidget
{
    Q_OBJECT
public:
    DbListDock(QWidget *parent=nullptr);

public Q_SLOTS:
    void setDatabaseList(const QStringList &databases, const QString &current);

Q_SIGNALS:
    void newConnection(const QString &database);
    void refreshRequested();

private Q_SLOTS:
    void customContextMenu(const QPoint& point);

private:
    QListWidget *m_list;
};

#endif // DBLIST_H
