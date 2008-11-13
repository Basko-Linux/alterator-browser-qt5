#ifndef QTBROWSER_AL_BUTTON_HH
#define QTBROWSER_AL_BUTTON_HH

#include <QPushButton>

#include "awidget.hh"
#include "al_widget.hh"

class AButton: public AWidget<QPushButton>
{
Q_OBJECT
Q_PROPERTY(QString altgroup READ alteratorGroup WRITE setAlteratorGroup)
public:
    AButton(QWidget *parent,const Qt::Orientation): AWidget<QPushButton>(parent) {};
    ~AButton() {};
};

class alButton: public alWidgetPre<AButton>
{
public:
	alButton(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
		alWidgetPre<AButton>(attr,WButton,id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
};

#endif
