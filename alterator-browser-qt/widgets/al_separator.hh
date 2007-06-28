#ifndef QTBROWSER_AL_SEPARATOR_HH
#define QTBROWSER_AL_SEPARATOR_HH

#include <QFrame>
#include "widgets.hh"

class alSeparator: public alWidgetPre<QFrame>
{
Q_OBJECT
public:
    alSeparator(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent, Qt::Orientation orientation);
    void setAttr(const QString& name,const QString& value);
};

#endif
