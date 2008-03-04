
#include <QTimerEvent>

#include "al_slider.hh"

ASlider::ASlider(QWidget *parent):
    QSlider(parent)
{
    timeout = 300;
    timer = -1;
    old_value = 0;
    connect(this, SIGNAL(valueChanged(int)),
	    this, SLOT(onValueChange(int)));
}

ASlider::~ASlider() {}

void ASlider::focusInEvent(QFocusEvent*)
{
    old_value = value();
}

void ASlider::focusOutEvent(QFocusEvent*)
{
    checkValueReallyChanged();
}

void ASlider::timerEvent(QTimerEvent* e)
{
    if( e->timerId() == timer )
	checkValueReallyChanged();
}

void ASlider::onValueChange(int new_value)
{
    if( timer >= 0 )
	killTimer( timer );
    timer = startTimer( timeout );
}

void ASlider::checkValueReallyChanged()
{
    int cur_value = value();
    if( old_value != cur_value )
    {
	if( timer >= 0 )
	    killTimer( timer );
	old_value = cur_value;
	emit valueReallyChanged();
    }
}

alSlider::alSlider(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent):
	alWidgetPre<ASlider>(attr,WSlider,id,parent)
{
    wnd_->setOrientation( Qt::Horizontal );
    wnd_->setTickPosition( QSlider::NoTicks );
}

void alSlider::setAttr(const QString& name,const QString& value)
{
	bool iok;
	int ivalue = value.toInt(&iok);

	if( "orientation" == name )
	{
	    if( "vertical" == value )
		wnd_->setOrientation(Qt::Vertical);
	    else
		wnd_->setOrientation(Qt::Horizontal);
	}
	else if( "minimum" == name )
	    wnd_->setMinimum( value.toInt() );
	else if( "maximum" == name )
	{
	    if(iok)
	    {
		wnd_->setMaximum( ivalue );
		wnd_->setSingleStep( (ivalue/64) + 1 );
		wnd_->setPageStep( (ivalue/16) + 1 );
	    }
	}
	else if( "step" == name )
	{
	    int step = value.toInt();
	    if(iok)
	    {
		wnd_->setSingleStep( ivalue );
		wnd_->setPageStep( ivalue*16 );
	    }
	}
	else if( "value" == name )
	{
	    if(iok)
		wnd_->setValue(ivalue);
	}
	else
	    alWidget::setAttr(name,value);
}

void alSlider::registerEvent(const QString& name)
{
	if ("changed" == name)
		connect(wnd_, SIGNAL(valueReallyChanged()), SLOT(onChange()));
}

QString alSlider::postData() const
{
	return QString(" (value . %1 )").arg(wnd_->value());
}
