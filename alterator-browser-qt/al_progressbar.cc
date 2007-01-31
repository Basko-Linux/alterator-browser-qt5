
#include "al_progressbar.hh"

void alProgressBar::setAttr(const QString& name,const QString& value)
{
    if( "value" == name )
	wnd_->setValue(value.toInt());
    else if( "maximum" == name )
	wnd_->setMaximum(value.toInt());
    else if( "minimum" == name )
	wnd_->setMinimum(value.toInt());
    else if( "text" == name )
    {
	if( value.isEmpty() )
	    wnd_->setFormat("%p%");
	else
	    wnd_->setFormat(value);
    }
    else
	alWidget::setAttr(name,value);
}
