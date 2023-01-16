#include "al_label.hh"
#include "a_pixmaps.hh"

void alLabel::setAttr(const QString& name,const QString& value)
{
	if (QStringLiteral("value") == name || QStringLiteral("text") == name)
		wnd_->setText(value);
	else if (QStringLiteral("text-wrap") == name)
		wnd_->setWordWrap(value == QStringLiteral("true"));
	else if (QStringLiteral("pixmap") == name)
		wnd_->setPixmap( getPixmap(value) );
	else
		alWidget::setAttr(name,value);
}
