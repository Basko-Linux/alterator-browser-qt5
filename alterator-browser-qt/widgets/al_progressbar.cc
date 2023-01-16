
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

    if( QStringLiteral("value") == name )
    {
	if(iok)
	    wnd_->setValue(value.toInt());
    }
    else if( QStringLiteral("maximum") == name )
    {
	if(iok)
	    wnd_->setMaximum(value.toInt());
    }
    else if( QStringLiteral("minimum") == name )
    {
	if(iok)
	    wnd_->setMinimum(value.toInt());
    }
    else if( QStringLiteral("text") == name )
    {
	if( value.isEmpty() )
	    wnd_->setFormat(QStringLiteral("%p%"));
	else
	    wnd_->setFormat(value);
    }
    else
	alWidget::setAttr(name,value);
}
