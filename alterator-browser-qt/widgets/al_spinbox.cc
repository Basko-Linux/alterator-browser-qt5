
#include "al_spinbox.hh"
alSpinBox::alSpinBox(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent):
    alWidgetPre<QDoubleSpinBox>(attr,WSpinBox,id,parent)
{
}

void alSpinBox::setAttr(const QString& name,const QString& value)
{
	if( "minimum" == name )
	    wnd_->setMinimum( value.toDouble() );
	else if( "maximum" == name )
	{
	    double mx = value.toDouble();
	    wnd_->setMaximum( mx );
	    wnd_->setSingleStep( (mx/64) + 1 );
	    //wnd_->setPageStep( (mx/16) + 1 );
	}
	else if( "step" == name )
	{
	    double step = value.toDouble();
	    wnd_->setSingleStep( step );
	    //wnd_->setPageStep( step*16 );
	}
	else if( "value" == name )
	    wnd_->setValue(value.toDouble());
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
