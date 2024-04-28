#ifndef WEBENGINE_H
#define WEBENGINE_H

class QWebEngineProfile;
class QObject;

class WebApi;

/**
 * Initialize a Web Engine Profile with all the KQueryBrowser customizations.
 */
QWebEngineProfile *initWebEngineProfile(WebApi *api, QObject *parent = nullptr);

#endif
