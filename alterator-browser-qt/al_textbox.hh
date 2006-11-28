#ifndef QTBROWSER_AL_TEXTBOX_HH
#define QTBROWSER_AL_TEXTBOX_HH

#include <QTextEdit>

#include "al_widget.hh"

class alTextBox: public alWidgetPre<QTextEdit>
{
public:
	alTextBox(const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};


#endif
