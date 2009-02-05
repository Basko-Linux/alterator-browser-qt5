#include "al_radio.hh"

ARadio::ARadio(QWidget *parent, const Qt::Orientation):
    AWidget<QRadioButton>(parent)
{}

ARadio::~ARadio()
{}

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
		connect(wnd_,SIGNAL( toggled(bool) ),SLOT(onToggle(bool)));
}

QString alRadio::postData() const
{
	return QString(" (state . ") + (wnd_->isChecked()?"#t":"#f")+" )";
}

