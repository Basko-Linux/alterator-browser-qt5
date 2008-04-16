#ifndef QTBROWSER_AL_LABEL_HH
#define QTBROWSER_AL_LABEL_HH

#include <QLabel>

#include "al_widget.hh"

class ALabel: public QLabel
{
public:
    ALabel(QWidget *parent, Qt::Orientation): QLabel(parent) {};
    ~ALabel() {};
};

class alLabel: public alWidgetPre<ALabel>
{
public:
	alLabel(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
		alWidgetPre<ALabel>(attr,WLabel,id,parent)
		{
		    //wnd_->setWordWrap( true );
		    //wnd_->setFrameStyle(QFrame::Sunken|QFrame::StyledPanel);
		}
	void setAttr(const QString& name,const QString& value);
};

#endif
