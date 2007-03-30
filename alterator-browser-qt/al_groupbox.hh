#ifndef QTBROWSER_AL_GROUPBOX_HH
#define QTBROWSER_AL_GROUPBOX_HH

#include <QGroupBox>

#include "al_widget.hh"

class alGroupBox: public alWidgetPre<QGroupBox>
{
public:
	alGroupBox(const QString& id,const QString& parent, Qt::Orientation orientation, bool checkable);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

class alVGroupBox: public alGroupBox
{
public:
	alVGroupBox(const QString& id,const QString& parent, bool checkable):
		alGroupBox(id,parent,Qt::Vertical, checkable)
	{}
};

class alHGroupBox: public alGroupBox
{
public:
	alHGroupBox(const QString& id,const QString& parent, bool checkable):
		alGroupBox(id,parent,Qt::Horizontal, checkable)
	{}
};

#endif
