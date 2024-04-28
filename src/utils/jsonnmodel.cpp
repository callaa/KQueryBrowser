#include "jsonnmodel.h"

#include <QTreeWidgetItem>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

using namespace Qt::StringLiterals;

static QList<QTreeWidgetItem*> makeObject(const QJsonObject &obj, QTreeWidgetItem *parent);

static QList<QTreeWidgetItem*> makeArray(const QJsonArray &array, QTreeWidgetItem *parent)
{
    QList<QTreeWidgetItem*> items;
    int idx = 0;
    for(const QJsonValue &val : array) {
        QTreeWidgetItem *item = new QTreeWidgetItem(parent);
        items.append(item);
        item->setData(0, Qt::DisplayRole, u"[%1]"_s.arg(idx));

        if(val.isArray()) {
            item->setData(1, Qt::DisplayRole, u"[%1]"_s.arg(val.toArray().count()));
            item->addChildren(makeArray(val.toArray(), item));
        } else if (val.isObject()) {
            item->setData(1, Qt::DisplayRole, u"{%1}"_s.arg(val.toObject().count()));
            item->addChildren(makeObject(val.toObject(), item));
        } else if(val.isNull()) {
            item->setData(1, Qt::DisplayRole, u"null"_s);
        } else {
            item->setData(1, Qt::DisplayRole, val.toVariant());
        }

        ++idx;
    }

    return items;
}

QList<QTreeWidgetItem*> makeObject(const QJsonObject &obj, QTreeWidgetItem *parent)
{
    QList<QTreeWidgetItem*> items;
    auto entry = obj.constBegin();
    while(entry != obj.constEnd()) {
        QTreeWidgetItem *item = new QTreeWidgetItem(parent);
        items.append(item);
        item->setData(0, Qt::DisplayRole, entry.key());

        if(entry.value().isArray()) {
            item->setData(1, Qt::DisplayRole, u"[%1]"_s.arg(entry.value().toArray().count()));
            item->addChildren(makeArray(entry.value().toArray(), item));
        } else if (entry.value().isObject()) {
            item->setData(1, Qt::DisplayRole, u"{%1}"_s.arg(entry.value().toObject().count()));
            item->addChildren(makeObject(entry.value().toObject(), item));
        } else if(entry.value().isNull()) {
            item->setData(1, Qt::DisplayRole, u"null"_s);
        } else {
            item->setData(1, Qt::DisplayRole, entry.value().toVariant());
        }

        ++entry;
    }

    return items;
}

QList<QTreeWidgetItem*> makeJsonModel(const QJsonDocument &doc)
{

    if(doc.isArray())
        return makeArray(doc.array(), nullptr);
    else
        return makeObject(doc.object(), nullptr);
}
