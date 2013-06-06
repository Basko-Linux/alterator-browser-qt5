#ifndef QTBROWSER_AL_DISPLAYSIZE_HH
#define QTBROWSER_AL_DISPLAYSIZE_HH

#include <QLabel>

#include "al_widget.hh"

class ADisplaySize: public AWidget<QLabel>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ADisplaySize(QWidget *parent, const Qt::Orientation): AWidget<QLabel>(parent) {};
    ~ADisplaySize() {};
};

class alDisplaySize: public alWidgetPre<ADisplaySize>
{
public:
	alDisplaySize(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
};

#endif
