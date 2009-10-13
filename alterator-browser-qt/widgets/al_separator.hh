#ifndef QTBROWSER_AL_SEPARATOR_HH
#define QTBROWSER_AL_SEPARATOR_HH

#include <QFrame>
#include "widgets.hh"

class ASeparator: public AWidget<QFrame>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ASeparator(QWidget *parent, const Qt::Orientation): AWidget<QFrame>(parent) {};
    ~ASeparator() {};
};

class alSeparator: public alWidgetPre<ASeparator>
{
Q_OBJECT
public:
    alSeparator(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent);
    void setAttr(const QString& name,const QString& value);
};

#endif
