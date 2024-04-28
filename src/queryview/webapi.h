#ifndef WEBAPI_H
#define WEBAPI_H

#include <QObject>
#include <QMap>
#include <QPointer>
#include <QMetaMethod>

class QJsonDocument;
class QJsonObject;

/**
 * The Web API methods made available to the web view.
 *
 * Making a call to "api:name" will invoke the named method.
 */
class WebApi : public QObject {
	Q_OBJECT
public:
	WebApi(QObject *parent);

	/**
	 * Make the method of the given object available to the Web API.
	 *
	 * Method signature should be: `QJsonDocument method(const QJsonObject &kwargs)`
	 */
	void registerMethod(const QString &name, QObject *obj, const char *methodName);

	/**
	 * Invoke the given method.
	 *
	 * Returns a null document if the method was not found.
	 */
    QJsonDocument invoke(const QString &name, const QJsonObject &params) const;

private Q_SLOTS:
    void methodTargetDestroyed(QObject *target);

private:
	struct ApiMethod {
        QPointer<QObject> obj;
		QMetaMethod method;
	};

	QMap<QString, ApiMethod> m_methods;
};

#endif
