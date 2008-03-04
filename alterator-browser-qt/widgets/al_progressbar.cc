
#include "al_progressbar.hh"

alProgressBar::alProgressBar(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
	alWidgetPre<QProgressBar>(attr,WProgressBar,id,parent)
{
    wnd_->setAlignment(Qt::AlignCenter);
}

void alProgressBar::setAttr(const QString& name,const QString& value)
{
    bool vok;
    int ivalue = value.toInt(&vok);

    if( "value" == name )
    {
	if(vok)
	    wnd_->setValue(value.toInt());
    }
    else if( "maximum" == name )
    {
	if(vok)
	    wnd_->setMaximum(value.toInt());
    }
    else if( "minimum" == name )
    {
	if(vok)
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
