#include "webapi.h"

#include <QDebug>
#include <QMetaType>
#include <QJsonObject>
#include <QJsonDocument>

WebApi::WebApi(QObject *parent)
    : QObject(parent)
{
}

void WebApi::registerMethod(const QString &name, QObject *obj, const char *methodName)
{
    if(m_methods.contains(name)) {
        qWarning() << "Web API method" << name << "already registered!";
        return;
    }

    const auto meta = obj->metaObject();
    int idx = meta->indexOfMethod(methodName);
    if(idx < 0) {
        qWarning() << "Web API method" << name << "of" << obj->objectName() << " cannot be registered. Function" << methodName << "not found.";
        return;
    }

    const auto mm = meta->method(idx);

    if(mm.returnType() != QMetaType::Type::QJsonDocument) {
        qWarning() << "Web API method" << name << "of" << obj->objectName() << " cannot be registered. Return type of function" << methodName << "is not QJsonDocument.";
        return;
    }

    if(mm.parameterCount() != 1 || mm.parameterType(0) != QMetaType::Type::QJsonObject) {
        qWarning() << "Web API method" << name << "of" << obj->objectName() << " cannot be registered. Parameters of function" << methodName << "should be (QJsonObject).";
        return;
    }

    m_methods.insert(name, {obj, mm});

    connect(obj, &QObject::destroyed, this, &WebApi::methodTargetDestroyed);
}

void WebApi::methodTargetDestroyed(QObject *target)
{
    QMutableMapIterator<QString, ApiMethod> i(m_methods);
    while(i.hasNext()) {
        i.next();
        if(i.value().obj.isNull() || i.value().obj == target) {
            qDebug() << "Removed deleted method" << i.key();
            i.remove();
        }
    }
}

QJsonDocument WebApi::invoke(const QString &name, const QJsonObject &params) const {
    if(!m_methods.contains(name)) {
        qWarning() << "No such Web API method:" << name;
        return QJsonDocument();
    }

    const auto m = m_methods[name];

    if(m.obj.isNull()) {
        qWarning() << "Target object of Web API method" << name << "has disappeared!";
        return QJsonDocument();
    }

    QJsonDocument ret;
    m.method.invoke(m.obj, qReturnArg(ret), params);

    return ret;
}
