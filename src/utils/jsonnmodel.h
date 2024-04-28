#ifndef JSONNMODEL_H
#define JSONNMODEL_H

#include <QList>

class QTreeWidgetItem;
class QJsonDocument;

QList<QTreeWidgetItem*> makeJsonModel(const QJsonDocument &doc);

#endif
