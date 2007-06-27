
#include "al_groupbox.hh"

alGroupBox::alGroupBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation, bool checkable):
	alWidgetPre<QGroupBox>(attr,(orientation==Qt::Vertical)?WVGroupBox:WHGroupBox,id,parent)
{
    wnd_->setCheckable(checkable);
    QBoxLayout *l;
    Qt::Orientation o = Utils::fixOrientation(orientation, Qt::Vertical);
    if( o == Qt::Horizontal )
	l = new QHBoxLayout(getViewWidget());
    else
	l = new QVBoxLayout(getViewWidget());
    l->setSpacing(5);
    l->setMargin(5);
}

void alGroupBox::setAttr(const QString& name,const QString& value)
{
	if ("title" == name)
		wnd_->setTitle(value);
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

