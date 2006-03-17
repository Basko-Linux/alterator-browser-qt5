#ifndef QTBROWSER_ALWIZARDFACE_HH
#define QTBROWSER_ALWIZARDFACE_HH

#include "widgets.hh"

class alWizardFace: public alWidgetPre<QWidget>
{
public:
	alWizardFace(const QString& id,const QString& parent):
		alWidgetPre<QWidget>(id,parent)
	{
	}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
