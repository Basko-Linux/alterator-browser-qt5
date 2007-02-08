#ifndef QTBROWSER_AL_COMBOBOX_HH
#define QTBROWSER_AL_COMBOBOX_HH

#include <QComboBox>

#include "al_widget.hh"

class alComboBox: public alWidgetPre<QComboBox>
{
	int counter_;
public:
	alComboBox(const QString& id,const QString& parent):
		alWidgetPre<QComboBox>(ComboBox,id,parent),
		counter_(0)
	{}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};


#endif
