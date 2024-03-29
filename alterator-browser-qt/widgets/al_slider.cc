
#include <QTimerEvent>

#include "al_slider.hh"

ASlider::ASlider(QWidget *parent, const Qt::Orientation):
    AWidget<QSlider>(parent)
{
    tmr = new QTimer(this);
    tmr->setSingleShot(true);
    tmr->setInterval(300);
    old_value = 0;
    connect(this, SIGNAL(valueChanged(int)),
	    this, SLOT(onValueChange(int)));
    connect(tmr, SIGNAL(timeout()),
	    this, SLOT(checkValueReallyChanged()));
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

void ASlider::onValueChange(int)
{
    tmr->start();
}

void ASlider::checkValueReallyChanged()
{
    int cur_value = value();
    if( old_value != cur_value )
    {
	old_value = cur_value;
	Q_EMIT valueReallyChanged();
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

	if( QStringLiteral("orientation") == name )
	{
	    if( QStringLiteral("vertical") == value )
		wnd_->setOrientation(Qt::Vertical);
	    else
		wnd_->setOrientation(Qt::Horizontal);
	}
	else if( QStringLiteral("minimum") == name )
	    wnd_->setMinimum( ivalue );
	else if( QStringLiteral("maximum") == name )
	{
	    if(iok)
	    {
		wnd_->setMaximum( ivalue );
		wnd_->setSingleStep( (ivalue/64) + 1 );
		wnd_->setPageStep( (ivalue/16) + 1 );
	    }
	}
	else if( QStringLiteral("step") == name )
	{
	    if(iok)
	    {
		wnd_->setSingleStep( ivalue );
		wnd_->setPageStep( ivalue*16 );
	    }
	}
	else if( QStringLiteral("value") == name )
	{
	    if(iok)
		wnd_->setValue(ivalue);
	}
	else
	    alWidget::setAttr(name,value);
}

void alSlider::registerEvent(const QString& name)
{
	if (QStringLiteral("changed") == name)
		connect(wnd_, SIGNAL(valueReallyChanged()), SLOT(onChange()));
}

QString alSlider::postData() const
{
	return QString(QStringLiteral(" (value . %1 )")).arg(wnd_->value());
}
