#ifndef QTBROWSER_CONNECTION_HH
#define QTBROWSER_CONNECTION_HH

#include <QDomDocument>

#include "sax.hh"

void initConnection(void (parser)(alCommand*));
void getDocument(void (parser)(alCommand*) ,
		 const QString& content = "(alterator-request action \"get\")");

#endif
