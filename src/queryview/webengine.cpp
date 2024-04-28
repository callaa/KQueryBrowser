#include "webengine.h"
#include "webapi.h"

#include <QDebug>
#include <QBuffer>
#include <QWebEngineProfile>
#include <QWebEngineUrlRequestInterceptor>
#include <QWebEngineUrlSchemeHandler>

#include <QWebEngineUrlRequestJob>
#include <QJsonDocument>
#include <QJsonObject>

/**
 * A request interceptor to block access to all outside resources.
 *
 * We use the web view only to render the query results. There is no need
 * to reference any external files or URLs. This might happen in case of
 * buggy content escaping/validation.
 */
class RequestInterceptor : public QWebEngineUrlRequestInterceptor {
public:
	RequestInterceptor(QObject *parent)
		: QWebEngineUrlRequestInterceptor(parent)
	{
	}

    void interceptRequest(QWebEngineUrlRequestInfo &info) override {
		const auto url = info.requestUrl();
		if(url.scheme() == QStringLiteral("qrc") && url.path().startsWith(QStringLiteral("/html/"))) {
			// These are our built-in HTML, style and JS files
			info.block(false);

		} else if(url.scheme() == QStringLiteral("api")) {
			// Internal API calls handled by ApiSchemaHandler
			info.block(false);

		} else {
			// No need to access anything else
			qWarning() << "Blocked request to" << url;
			info.block(true);
		}
	}
};

class ApiSchemaHandler : public QWebEngineUrlSchemeHandler {
    WebApi *m_api;

public:
    ApiSchemaHandler(WebApi *api, QObject *parent)
        : QWebEngineUrlSchemeHandler(parent), m_api(api)
	{
	}

    void requestStarted(QWebEngineUrlRequestJob *job) override {
		const auto url = job->requestUrl();

        qDebug() << "API request" << url;

		if(job->requestMethod() != QByteArrayLiteral("POST")) {
			qWarning() << job->requestMethod() << url << ": expected POST method";
			job->fail(QWebEngineUrlRequestJob::RequestFailed);
			return;
		}

		// Request body should be the JSON encoded API call parameters
		QJsonParseError parseError;
        auto reqbody = job->requestBody();
        reqbody->open(QIODevice::ReadOnly);
        // Note: size limit is rather arbitrary
        auto doc = QJsonDocument::fromJson(readBytes(reqbody, 1024 * 1024 * 100), &parseError);
		if(doc.isNull()) {
            qWarning() << url << "Error parsing internal API request body JSON:" << parseError.errorString();
			job->fail(QWebEngineUrlRequestJob::RequestFailed);
			return;
		}

		if(!doc.isObject()) {
            qWarning() << url << "Expected JSON object as internal API request content";
			job->fail(QWebEngineUrlRequestJob::RequestFailed);
			return;
		}

		// Dispatch API call
        const auto result = m_api->invoke(url.path(), doc.object());

		// Return call response in JSON format
		auto buf = new QBuffer(job);
		buf->open(QBuffer::ReadWrite);
        buf->write(result.toJson(QJsonDocument::Compact));
		buf->seek(0);
		job->reply(QByteArrayLiteral("application/json"), buf);
	}

private:
    static QByteArray readBytes(QIODevice *device, qint64 maxLen) {
        QByteArray output;
        char buf[1024 * 64];
        do {
            // note: as presently implemented, maxLen is a rough limit, which is fine for this use case
            const auto r = device->read(buf, qint64(sizeof buf));
            if(r <= 0)
                break;

            output.append(buf, r);
        } while(output.length() < maxLen);

        return output;
    }
};

QWebEngineProfile *initWebEngineProfile(WebApi *api, QObject *parent) {
	auto wep = new QWebEngineProfile(parent);
	wep->setUrlRequestInterceptor(new RequestInterceptor(wep));    
    wep->installUrlSchemeHandler("api", new ApiSchemaHandler(api, wep));
	return wep;
}
