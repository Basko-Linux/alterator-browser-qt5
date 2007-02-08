#ifndef QTBROWSER_AL_RADIO_HH
#define QTBROWSER_AL_RADIO_HH

#include <QRadioButton>

#include "al_widget.hh"

class alRadio: public alWidgetPre<QRadioButton>
{
public:
	alRadio(const QString& id,const QString& parent):
		alWidgetPre<QRadioButton>(Radio,id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
