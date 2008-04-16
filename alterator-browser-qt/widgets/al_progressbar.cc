
#include "al_progressbar.hh"

alProgressBar::alProgressBar(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
	alWidgetPre<AProgressBar>(attr,WProgressBar,id,parent)
{
    wnd_->setAlignment(Qt::AlignCenter);
}

void alProgressBar::setAttr(const QString& name,const QString& value)
{
    bool iok;
    int ivalue = value.toInt(&iok);

    if( "value" == name )
    {
	if(iok)
	    wnd_->setValue(value.toInt());
    }
    else if( "maximum" == name )
    {
	if(iok)
	    wnd_->setMaximum(value.toInt());
    }
    else if( "minimum" == name )
    {
	if(iok)
	    wnd_->setMinimum(value.toInt());
    }
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
