#ifndef QTBROWSER_AL_LABEL_HH
#define QTBROWSER_AL_LABEL_HH

#include <QLabel>

#include "al_widget.hh"

class alLabel: public alWidgetPre<QLabel>
{
public:
	alLabel(const QString& id,const QString& parent):
		alWidgetPre<QLabel>(Label, id,parent)
		{
		    //wnd_->setWordWrap( true );
		    //wnd_->setFrameStyle(QFrame::Sunken|QFrame::StyledPanel);
		}
	void setAttr(const QString& name,const QString& value);
};

#endif
