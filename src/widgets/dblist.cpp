#include "dblist.h"

#include <QListWidget>
#include <QMenu>
#include <KLocalizedString>

DbListDock::DbListDock(QWidget *parent)
    : QDockWidget(i18n("Databases"), parent)
{
    m_list = new QListWidget(this);
    m_list->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(m_list, &QListWidget::customContextMenuRequested, this, &DbListDock::customContextMenu);
    setWidget(m_list);
}

void DbListDock::setDatabaseList(const QStringList &databases, const QString &current)
{
    m_list->clear();
    for(const QString &db : databases) {
        QListWidgetItem *item = new QListWidgetItem(db, m_list);
        if(db == current) {
            QFont font = item->font();
            font.setBold(true);
            item->setFont(font);
        }
        m_list->addItem(item);
    }
}

void DbListDock::customContextMenu(const QPoint &point)
{
    QMenu menu;

    QListWidgetItem *item = m_list->itemAt(point);
    QAction *newConnect=nullptr;
    if(item) {
        newConnect = menu.addAction(tr("New connection"));
    }

    if(!menu.isEmpty())
        menu.addSeparator();

    QAction *refreshAct = menu.addAction(tr("Refresh"));

    QAction *a = menu.exec(m_list->mapToGlobal(point));
    if(a) {
        if(a==refreshAct)
            Q_EMIT refreshRequested();
        else if(a==newConnect)
            Q_EMIT newConnection(item->text());
    }

}
