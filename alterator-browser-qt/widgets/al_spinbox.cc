
#include "al_spinbox.hh"
alSpinBox::alSpinBox(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent):
    alWidgetPre<ASpinBox>(attr,WSpinBox,id,parent)
{
}

void alSpinBox::setAttr(const QString& name,const QString& value)
{
	bool dok;
	double dvalue = value.toInt(&dok);

	if( "minimum" == name )
	{
	    if(dok)
		wnd_->setMinimum(dvalue);
	}
	else if( "maximum" == name )
	{
	    if(dok)
	    {
		wnd_->setMaximum( dvalue );
		wnd_->setSingleStep( (dvalue/64) + 1 );
		//wnd_->setPageStep( (dvalue/16) + 1 );
	    }
	}
	else if( "step" == name )
	{
	    if(dok)
	    {
		wnd_->setSingleStep( dvalue );
		//wnd_->setPageStep( dvalue*16 );
	    }
	}
	else if( "value" == name )
	{
	    if(dok)
		wnd_->setValue(dvalue);
	}
	else
	    alWidget::setAttr(name,value);
}

void alSpinBox::registerEvent(const QString& name)
{
	if ("changed" == name)
		connect(wnd_, SIGNAL(editingFinished()), SLOT(onChange()));
}

QString alSpinBox::postData() const
{
	return QString(" (value . %1 )").arg(wnd_->value());
}
