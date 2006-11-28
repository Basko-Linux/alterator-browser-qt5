#ifndef QTBROWSER_AL_EDIT_HH
#define QTBROWSER_AL_EDIT_HH

#include <QLineEdit>

#include "al_widget.hh"

class alEdit: public alWidgetPre<QLineEdit>
{
public:
	alEdit(const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
