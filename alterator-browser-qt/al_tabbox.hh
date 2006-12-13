#ifndef QTBROWSER_AL_TABBOX_HH
#define QTBROWSER_AL_TABBOX_HH

#include <QTabWidget>

#include "al_widget.hh"

class alTabBox: public alWidgetPre<QTabWidget>
{
public:
	QString current_;
	alTabBox(const QString& id,const QString& parent):
		alWidgetPre<QTabWidget>(TabBox,id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
};


class alTabPage: public alWidgetPre<QWidget>
{
	alTabBox  *tabbox_;
	int idx_;
public:
	alTabPage(const QString& id,const QString& parent):
		alWidgetPre<QWidget>(TabPage,id,":reparent:"+parent),
		tabbox_(getParentTabBox(parent)),
		idx_(tabbox_?static_cast<QTabWidget*>(tabbox_->getWidget())->addTab(wnd_,""):-1)
	{
	    QVBoxLayout *bl = new QVBoxLayout(getViewWidget());
	    bl->setSpacing(5);
	    bl->setMargin(5);
	}
	void setAttr(const QString& name,const QString& value);
private:
	static
	alTabBox* getParentTabBox(const QString& parent)
	{
		if (!elements.contains(parent)) return 0;
		return qobject_cast<alTabBox*>(elements[parent]);
	}
};

#endif
