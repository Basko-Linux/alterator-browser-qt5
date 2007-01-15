#ifndef QTBROWSER_AL_SEPARATOR_HH
#define QTBROWSER_AL_SEPARATOR_HH

#include <QFrame>
#include "widgets.hh"

class alSeparator: public alWidgetPre<QFrame>
{
Q_OBJECT
public:
    alSeparator(const QString& id, const QString& parent);
    void setAttr(const QString& name,const QString& value);
};

#endif
