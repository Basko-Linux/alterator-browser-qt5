
#include <cmath>

#include "al_spinbox.hh"

#define SP_SLIDER_MAX 65535
#define SP_SLIDER_MIN 0
#define SP_POW 1.3

ASpinBox::ASpinBox(QWidget *parent, const Qt::Orientation o):
    AWidget<QWidget>(parent)
{
    slider_own_changes = false;
    spinbox_own_changes = false;
    slider = new QSlider(this);
    slider->setOrientation(Qt::Horizontal);
    slider->setTracking(true);
    slider->setMinimum(SP_SLIDER_MIN);
    slider->setMaximum(SP_SLIDER_MAX);
    slider->setSingleStep(1);
    slider->setPageStep(SP_SLIDER_MAX/16);
    slider->setVisible(false);
    spinbox = new QSpinBox(this);
    setFocusProxy(spinbox);
    Qt::Orientation orient = Utils::fixOrientation(o, Qt::Horizontal);
    if( orient == Qt::Horizontal )
    {
	blayout = new QHBoxLayout(this);
	blayout->addWidget(slider);
	blayout->addWidget(spinbox);
    }
    else
    {
	blayout = new QVBoxLayout(this);
	blayout->addWidget(spinbox);
	blayout->addWidget(slider);
    }
    blayout->setMargin(0);
    blayout->setSpacing(0);

    setMinimum(0);
    setMaximum(INT_MAX);
    setStep(8);
    setValue(0);

    connect(slider, SIGNAL(valueChanged(int)), this, SLOT(onSliderValueChange(int)));
    connect(slider, SIGNAL(sliderReleased()), this, SLOT(onUserSliderRelease()));
    connect(spinbox, SIGNAL(valueChanged(int)), this, SLOT(onSpinBoxValueChange(int)));
    connect(spinbox, SIGNAL(editingFinished()), this, SLOT(onUserSpinBoxRelease()));
}

ASpinBox::~ASpinBox()
{}

void ASpinBox::onSliderValueChange(int newval)
{
    if( slider_own_changes ) return;

    int slider_min = slider->minimum();
    int slider_range = slider->maximum() - slider_min;
    int slider_val = newval - slider_min;

    int spin_step = spinbox->singleStep();
    int spin_min = spinbox->minimum();
    int spin_range = spinbox->maximum() - spin_min;

    double spin_new = ((double)slider_val/slider_range)*spin_range+spin_min;
    int spin_finished = floor(spin_new/spin_step+0.5)*spin_step;

    spinbox_own_changes = true;
    spinbox->setValue(spin_finished);
    spinbox_own_changes = false;
}

void ASpinBox::onSpinBoxValueChange(int newval)
{
    if( spinbox_own_changes ) return;

    int slider_min = slider->minimum();
    int slider_range = slider->maximum() - slider_min;

    int spin_min = spinbox->minimum();
    int spin_range = spinbox->maximum() - spin_min;
    int spin_val = newval - spin_min;

    int slider_new = ((double)spin_val/spin_range)*slider_range+slider_min;

    slider_own_changes = true;
    slider->setValue(slider_new);
    slider_own_changes = false;
}

void ASpinBox::onUserSliderRelease()
{
    emit editingFinished();
}

void ASpinBox::onUserSpinBoxRelease()
{
    double spin_val = spinbox->value();
    int spin_step = spinbox->singleStep();
    int spin_finished = floor(spin_val/spin_step+0.5)*spin_step;
    if( spin_val != spin_finished )
    {
	spinbox_own_changes = true;
	spinbox->setValue(spin_finished);
        spinbox_own_changes = false;
    }
    emit editingFinished();
}

void ASpinBox::setSliderVisibility(bool visible)
{
    slider->setVisible(visible);
}

void ASpinBox::setValue(int ivalue)
{
    spinbox->setValue( ivalue );
}

void ASpinBox::setMinimum(int ivalue)
{
    spinbox->setMinimum( ivalue );
}

void ASpinBox::setMaximum(int ivalue)
{
    spinbox->setMaximum( ivalue );
}

void ASpinBox::setStep(int ivalue)
{
    spinbox->setSingleStep( (ivalue<=0)? 1: ivalue );
}

int ASpinBox::value()
{
    return spinbox->value();
}

// alSpinBox
alSpinBox::alSpinBox(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent):
    alWidgetPre<ASpinBox>(attr,WSpinBox,id,parent)
{
}

void alSpinBox::setAttr(const QString& name,const QString &value)
{
	bool ok;
	int ivalue = value.toInt(&ok);

	if( "minimum" == name )
	{
	    if(ok)
		wnd_->setMinimum(ivalue);
	}
	else if( "maximum" == name )
	{
	    if(ok)
		wnd_->setMaximum( ivalue );
	}
	else if( "step" == name )
	{
	    if(ok)
		wnd_->setStep( ivalue );
	}
	else if( "value" == name )
	{
	    if(ok)
		wnd_->setValue(ivalue);
	}
	else if( "expanded" == name )
	{
	    wnd_->setSliderVisibility(value != "false");
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
