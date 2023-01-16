#include "al_checkbox.hh"

alCheckBox::alCheckBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
    alWidgetPre<ACheckBox>(attr,WCheckBox,id,parent)
{
    connect(wnd_, SIGNAL(stateChanged(int)), this, SLOT(onUpdate(int)));
}

void alCheckBox::setAttr(const QString& name,const QString& value)
{
	if (QStringLiteral("text") == name)
		wnd_->setText(value);
	else if (QStringLiteral("state") == name || QStringLiteral("value") == name)
		wnd_->setChecked(QStringLiteral("true") ==  value);
	else
		alWidget::setAttr(name,value);
}

void alCheckBox::registerEvent(const QString& name)
{
	if (QStringLiteral("toggled") == name)
		connect(wnd_,SIGNAL( stateChanged(int) ),SLOT(onToggle(int)));
	else if (QStringLiteral("changed") == name)
		connect(wnd_,SIGNAL( stateChanged(int) ),SLOT(onChange(int)));
}

QString alCheckBox::postData() const
{
    QString ret;
    ret += QString(QStringLiteral(" (state . %1 )")).arg(wnd_->isChecked()? QStringLiteral("#t"): QStringLiteral("#f"));
    ret += QString(QStringLiteral(" (value . %1 )")).arg(wnd_->isChecked()? QStringLiteral("#t"): QStringLiteral("#f"));
    return ret;
}
