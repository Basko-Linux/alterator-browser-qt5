#include "al_radio.hh"

#include "browser.hh"

ARadio::ARadio(QWidget *parent, const Qt::Orientation):
    AWidget<QRadioButton>(parent)
{
}

ARadio::~ARadio()
{}

void ARadio::onToggle(bool)
{
    if( eventRegistered(BrowserEventToggled) )
	browser->emitEvent(getId(), BrowserEventToggled, AlteratorRequestDefault);
}

void alRadio::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("state" == name)
		wnd_->setChecked("true" ==  value);
	else
		alWidget::setAttr(name,value);
}

void alRadio::registerEvent(const QString& name)
{
    if ("toggled" == name)
	wnd_->setEventRegistered(id_, BrowserEventToggled);

    if( wnd_->eventRegistered(BrowserEventToggled) )
	connect(wnd_, SIGNAL(toggled(bool)), wnd_, SLOT(onToggle(bool)));
}

QString alRadio::postData() const
{
	return QString(" (state . ") + (wnd_->isChecked()?"#t":"#f")+" )";
}

