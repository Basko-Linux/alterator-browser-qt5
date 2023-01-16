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
	g_browser->emitEvent(getId(), BrowserEventToggled, AlteratorRequestDefault);
    if( eventRegistered(BrowserEventChanged) )
	g_browser->emitEvent(getId(), BrowserEventChanged, AlteratorRequestDefault);
}

void alRadio::setAttr(const QString& name,const QString& value)
{
	if (QStringLiteral("text") == name)
		wnd_->setText(value);
	else if (QStringLiteral("state") == name)
		wnd_->setChecked(QStringLiteral("true") ==  value);
	else
		alWidget::setAttr(name,value);
}

void alRadio::registerEvent(const QString& name)
{
    if (QStringLiteral("toggled") == name)
	wnd_->setEventRegistered(id_, BrowserEventToggled);
    else if (QStringLiteral("changed") == name)
	wnd_->setEventRegistered(id_, BrowserEventChanged);

    if( wnd_->eventRegistered(BrowserEventToggled) || wnd_->eventRegistered(BrowserEventChanged) )
	connect(wnd_, SIGNAL(toggled(bool)), wnd_, SLOT(onToggle(bool)));
}

QString alRadio::postData() const
{
	return QString(QStringLiteral(" (state . ")) + (wnd_->isChecked()? QStringLiteral("#t"): QStringLiteral("#f")) + QStringLiteral(" )");
}

