
#include "al_spinbox.hh"

void alSpinBox::setAttr(const QString& name,const QString& value)
{
	if( "minimum" == name )
	    wnd_->setMinimum( value.toInt() );
	else if( "maximum" == name )
	{
	    int mx = value.toInt();
	    wnd_->setMaximum( mx );
	    wnd_->setSingleStep( (mx/64) + 1 );
	}
	else if( "step" == name )
	{
	    int step = value.toInt();
	    wnd_->setSingleStep( step );
	}
	else if( "value" == name )
	    wnd_->setValue(value.toInt());
	else
	    alWidget::setAttr(name,value);
}

void alSpinBox::registerEvent(const QString& name)
{
	if ("changed" == name)
		connect(wnd_, SIGNAL(valueReallyChanged()), SLOT(onChange()));
}

QString alSpinBox::postData() const
{
	return QString(" (value . %1 )").arg(wnd_->value());
}
