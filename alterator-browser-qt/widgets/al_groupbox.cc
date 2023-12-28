
#include "al_groupbox.hh"

AGroupBox::AGroupBox(QWidget *parent, const Qt::Orientation o):
    AWidget<QGroupBox>(parent)
{
    setFlat(false);
    setCheckable(false);
    Qt::Orientation orient = Utils::fixOrientation(o, Qt::Vertical);
    if( orient == Qt::Horizontal )
	l = new QHBoxLayout(this);
    else
	l = new QVBoxLayout(this);
    l->setSpacing(5);
    l->setContentsMargins(5,5,5,5);
}

AGroupBox::~AGroupBox()
{}

void AGroupBox::setMyTitle(const QString& txt)
{
    setTitle(txt);
}

// alGroupBox
alGroupBox::alGroupBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
		alWidgetPre<AGroupBox>(attr,attr.value(QStringLiteral("type")).t,id,parent)
{
    switch( type() )
    {
	case WVGroupBox:
	case WHGroupBox:
	case WGroupBox:
	{
	    if( attr.contains(QStringLiteral("checked")) )
		wnd_->setCheckable(attr.value(QStringLiteral("checked")).b);
	    break;
	}
	default:
	    break;
    }
}

void alGroupBox::setAttr(const QString& name,const QString& value)
{
    if (QStringLiteral("title") == name)
	wnd_->setMyTitle(value);
    else if (QStringLiteral("state") == name)
	wnd_->setChecked(QStringLiteral("true") ==  value);
    else
	alWidget::setAttr(name,value);
}

void alGroupBox::registerEvent(const QString& name)
{
    if (QStringLiteral("toggled") == name)
	connect(wnd_,SIGNAL( toggled(bool) ),SLOT(onToggle(bool)));
}


QString alGroupBox::postData() const
{
    if (wnd_->isCheckable())
	return QString(QStringLiteral(" (state . %1 )")).arg(wnd_->isChecked()? QStringLiteral("#t"): QStringLiteral("#f"));
    else
	return QString();
}
