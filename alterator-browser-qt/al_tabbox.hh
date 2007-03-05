#ifndef QTBROWSER_AL_TABBOX_HH
#define QTBROWSER_AL_TABBOX_HH

#include <QTabWidget>

#include "al_widget.hh"

class alTabBox: public alWidgetPre<QTabWidget>
{
public:
	alTabBox(const QString& id,const QString& parent, Qt::Orientation orientation);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
	Qt::Orientation getOrientation() { return orientation_; };
private:
	Qt::Orientation orientation_;
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

#endif
