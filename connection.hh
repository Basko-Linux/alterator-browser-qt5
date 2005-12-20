#ifndef QTBROWSER_CONNECTION_HH
#define QTBROWSER_CONNECTION_HH

#include <QDomDocument>

void initConnection(void (parser)(const QDomElement&));
void getDocument(void (parser)(const QDomElement&) ,
		 const QString& content = "(alterator-request action \"get\")");

#endif
