#include "al_button.hh"
#include "a_pixmaps.hh"

void alButton::setAttr(const QString& name,const QString& value)
{
	if (QStringLiteral("text") == name)
		wnd_->setText(value);
	else if (QStringLiteral("pixmap") == name)
		wnd_->setIcon(QIcon(getPixmap(value)));
	else
		alWidget::setAttr(name,value);
}

void alButton::registerEvent(const QString& name)
{
	if (QStringLiteral("clicked") == name)
		connect(wnd_,SIGNAL( clicked(bool) ),SLOT(onClick(bool)));
}

