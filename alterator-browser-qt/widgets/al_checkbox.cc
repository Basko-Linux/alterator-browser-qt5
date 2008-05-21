#include "al_checkbox.hh"

alCheckBox::alCheckBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
    alWidgetPre<ACheckBox>(attr,WCheckBox,id,parent)
{
    connect(wnd_, SIGNAL(stateChanged(int)), this, SLOT(onUpdate(int)));
}

void alCheckBox::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("state" == name || "value" == name)
		wnd_->setChecked("true" ==  value);
	else
		alWidget::setAttr(name,value);
}

void alCheckBox::registerEvent(const QString& name)
{
	if ("toggled" == name)
		connect(wnd_,SIGNAL( stateChanged(int) ),SLOT(onToggle(int)));
}

QString alCheckBox::postData() const
{
	return QString(" (state . %1 )").arg(wnd_->isChecked()?"#t":"#f");
	return QString(" (value . %1 )").arg(wnd_->isChecked()?"#t":"#f");
}
