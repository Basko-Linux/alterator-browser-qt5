#include "al_label.hh"
#include "a_pixmaps.hh"

void alLabel::setAttr(const QString& name,const QString& value)
{
	if ("value" == name || "text" == name)
		wnd_->setText(value);
	else if ("text-wrap" == name)
		wnd_->setWordWrap(value == "true");
	else if ("pixmap" == name)
		wnd_->setPixmap( getPixmap(value) );
	else
		alWidget::setAttr(name,value);
}
