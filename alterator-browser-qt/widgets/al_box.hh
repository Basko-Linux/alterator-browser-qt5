
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "al_widget.hh"

class alBox: public alWidgetPre<QWidget>
{
public:
	alBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation);
	void setAttr(const QString& name,const QString& value);
};

class alVBox: public alBox
{
public:
	alVBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
		alBox(attr,id,parent,Qt::Vertical)
	{
	}
};

class alHBox: public alBox
{
public:
	alHBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
		alBox(attr,id,parent,Qt::Horizontal)
	{
	}
};
