
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


void alSlider::setAttr(const QString& name,const QString& value)
{
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
	    int mx = value.toInt();
	    wnd_->setMaximum( mx );
	    wnd_->setSingleStep( (mx/64) + 1 );
	    wnd_->setPageStep( (mx/16) + 1 );
	}
	else if( "step" == name )
	{
	    int step = value.toInt();
	    wnd_->setSingleStep( step );
	    wnd_->setPageStep( step*16 );
	}
	else if( "value" == name )
	    wnd_->setValue(value.toInt());
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
