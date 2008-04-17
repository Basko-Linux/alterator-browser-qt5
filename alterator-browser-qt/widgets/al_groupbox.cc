
#include "al_groupbox.hh"

extern Enums *enums;

AGroupBox::AGroupBox(QWidget *parent, const Qt::Orientation o):
    QGroupBox(parent)
{
    setFlat(false);
    setCheckable(false);
    Qt::Orientation orient = Utils::fixOrientation(o, Qt::Vertical);
    if( orient == Qt::Horizontal )
	l = new QHBoxLayout(this);
    else
	l = new QVBoxLayout(this);
    l->setSpacing(5);
    l->setMargin(5);
}

AGroupBox::~AGroupBox()
{}

void AGroupBox::setMyTitle(const QString& txt)
{
    setTitle(txt);
}

// alGroupBox
alGroupBox::alGroupBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
		alWidgetPre<AGroupBox>(attr,attr[AltReqParamWType].t,id,parent)
{
    switch( type() )
    {
	case WVGroupBox:
	case WHGroupBox:
	case WGroupBox:
	{
	    wnd_->setCheckable(attr[AltReqParamWChecked].b);
	    break;
	}
	default:
	    break;
    }
}

void alGroupBox::setAttr(const QString& name,const QString& value)
{
    if ("title" == name)
	wnd_->setMyTitle(value);
    else if ("state" == name)
	wnd_->setChecked("true" ==  value);
    else
	alWidget::setAttr(name,value);
}

void alGroupBox::registerEvent(const QString& name)
{
    if ("toggled" == name)
	connect(wnd_,SIGNAL( toggled(bool) ),SLOT(onToggle(bool)));
}


QString alGroupBox::postData() const
{
    if (wnd_->isCheckable())
	return QString(" (state . %1 )").arg(wnd_->isChecked()?"#t":"#f");
    else
	return "";
}
