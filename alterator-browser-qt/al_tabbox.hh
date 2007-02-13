#ifndef QTBROWSER_AL_TABBOX_HH
#define QTBROWSER_AL_TABBOX_HH

#include <QTabWidget>

#include "al_widget.hh"

class alTabBox: public alWidgetPre<QTabWidget>
{
public:
	Qt::Orientation orientation_;
	QString current_;
	alTabBox(const QString& id,const QString& parent, Qt::Orientation orientation);
	void setAttr(const QString& name,const QString& value);
	Qt::Orientation getOrientation() { return orientation_; };
};

class alHTabBox: public alTabBox
{
public:
	alHTabBox(const QString& id,const QString& parent):
		alTabBox(id,parent,Qt::Horizontal)
	{}
};

class alVTabBox: public alTabBox
{
public:
	alVTabBox(const QString& id,const QString& parent):
		alTabBox(id,parent,Qt::Vertical)
	{}
};


class alTabPage: public alWidgetPre<QWidget>
{
	alTabBox  *tabbox_;
	int idx_;
public:
	alTabPage(const QString& id,const QString& parent, Qt::Orientation orientation);
	void setAttr(const QString& name,const QString& value);
private:
	static
	alTabBox* getParentTabBox(const QString& parent)
	{
		if (!elements.contains(parent)) return 0;
		return qobject_cast<alTabBox*>(elements[parent]);
	}
};

class alHTabPage: public alTabPage
{
public:
	alHTabPage(const QString& id,const QString& parent):
		alTabPage(id,parent,Qt::Horizontal)
	{}
};

class alVTabPage: public alTabPage
{
public:
	alVTabPage(const QString& id,const QString& parent):
		alTabPage(id,parent,Qt::Vertical)
	{}
};

#endif
