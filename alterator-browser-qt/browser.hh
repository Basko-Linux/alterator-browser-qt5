#ifndef QT_BROWSER_HH
#define QT_BROWSER_HH

#include <QTimer>

#include "sax.hh"

void getDocParser(alCommand *cmd);
void emitEvent(const QString& id,const QString& type);

#endif
