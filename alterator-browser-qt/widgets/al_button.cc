#include "al_button.hh"
#include "a_pixmaps.hh"

void alButton::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("pixmap" == name)
		wnd_->setIcon(QIcon(getPixmap(value)));
	else
		alWidget::setAttr(name,value);
}

void alButton::registerEvent(const QString& name)
{
	if ("clicked" == name)
		connect(wnd_,SIGNAL( clicked(bool) ),SLOT(onClick(bool)));
}

