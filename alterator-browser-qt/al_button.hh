#ifndef QTBROWSER_AL_BUTTON_HH
#define QTBROWSER_AL_BUTTON_HH

#include <QPushButton>

#include "al_widget.hh"

class alButton: public alWidgetPre<QPushButton>
{
public:
	alButton(const QString& id,const QString& parent):
		alWidgetPre<QPushButton>(Button, id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
};

#endif
