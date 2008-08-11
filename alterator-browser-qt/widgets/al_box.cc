
#include "al_box.hh"
#include "enums.hh"

ABox::ABox(QWidget *parent, const Qt::Orientation o):
    QWidget(parent)
{
    Qt::Orientation orient = Utils::fixOrientation(o, Qt::Vertical);
    if( orient == Qt::Horizontal )
	l = new QHBoxLayout(this);
    else
	l = new QVBoxLayout(this);
    l->setSpacing(5);
    l->setMargin(0);
}

ABox::~ABox()
{}

void ABox::setMyTitle(const QString& txt)
{
    //setTitle(txt);
}

// alBox
alBox::alBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
		alWidgetPre<ABox>(attr,attr[AltReqParamWType].t,id,parent)
{
#if 0
    switch( type() )
    {
	case WVBox:
	case WHBox:
	case WVGroupBox:
	case WHGroupBox:
	{
	    wnd_->setMyTitle(enums->widgetToStr(type()));
	}
	default: break;
    }
#endif
}
