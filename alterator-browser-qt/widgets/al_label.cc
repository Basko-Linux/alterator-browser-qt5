#include "al_label.hh"
#include "a_pixmaps.hh"

void alLabel::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	if ("text-wrap" == name)
		wnd_->setWordWrap(value == "true" || value == "#t");
	else if ("pixmap" == name)
		wnd_->setPixmap( getPixmap(value) );
//	else if ("align" == name)
//	{
//		wnd_->setAlignment(Utils::convertAlign(value));
//	}
	else
		alWidget::setAttr(name,value);
}

