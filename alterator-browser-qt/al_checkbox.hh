#ifndef QTBROWSER_AL_CHECKBOX_HH
#define QTBROWSER_AL_CHECKBOX_HH

#include <QCheckBox>

#include "al_widget.hh"

class alCheckBox: public alWidgetPre<QCheckBox>
{
public:
	alCheckBox(const QString& id,const QString& parent):
		alWidgetPre<QCheckBox>(CheckBox,id,parent)
	{
	    connect(wnd_, SIGNAL(stateChanged(int)), this, SLOT(onUpdate(int)));
	}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
	QString getValue() { return  (wnd_->isChecked()?"true":"false"); }
};

#endif
