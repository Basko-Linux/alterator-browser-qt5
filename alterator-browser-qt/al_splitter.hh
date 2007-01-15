#ifndef QTBROWSER_AL_SPLITTER_HH
#define QTBROWSER_AL_SPLITTER_HH

#include <QFrame>
#include "widgets.hh"

class alSplitter: public alWidgetPre<QFrame>
{
Q_OBJECT
public:
    alSplitter(const QString& id, const QString& parent);
    void setAttr(const QString& name,const QString& value);
};

#endif
