
#include <QHBoxLayout>
#include <QVBoxLayout>

#include "al_widget.hh"

class ASimpleBox: public QWidget
{
public:
    ASimpleBox(QWidget *parent, Qt::Orientation): QWidget(parent) {};
    ~ASimpleBox() {};
};

class alBox: public alWidgetPre<ASimpleBox>
{
public:
	alBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation);
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
