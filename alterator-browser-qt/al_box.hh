
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "al_widget.hh"

class alBox: public alWidgetPre<QWidget>
{
public:
	alBox(const QString& id,const QString& parent, Qt::Orientation orientation);
	void setAttr(const QString& name,const QString& value);
};

class alVBox: public alBox
{
public:
	alVBox(const QString& id,const QString& parent):
		alBox(id,parent,Qt::Vertical)
	{}
};

class alHBox: public alBox
{
public:
	alHBox(const QString& id,const QString& parent):
		alBox(id,parent,Qt::Horizontal)
	{}
};
