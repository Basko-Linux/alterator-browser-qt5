#ifndef QTBROWSER_CONNECTION_HH
#define QTBROWSER_CONNECTION_HH

#include <QDomDocument>

#include "sax.hh"

typedef void (*parserfunc)(alCommand*);

void initConnection(parserfunc);
void getDocument(parserfunc,
		 const QString& content = "(alterator-request action \"get\")");

#endif
