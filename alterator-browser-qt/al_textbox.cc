
#include "utils.hh"
#include "al_textbox.hh"

alTextBox::alTextBox(const QString& id,const QString& parent):
		alWidgetPre<QTextEdit>(TextBox,id,parent)
{
}

void alTextBox::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setPlainText(value);
	else if ("text-append" == name)
		wnd_->append(value);
	else if ("append-text" == name)
		wnd_->append(value);
	else if ("alterability" == name)
		wnd_->setReadOnly("false" == value);
	else
		alWidget::setAttr(name,value);
}

void alTextBox::registerEvent(const QString& name)
{
	if ("changed" == name)
		connect(wnd_,SIGNAL( textChanged(const QString&) ),SLOT(onChange(const QString&)));
}

QString alTextBox::postData() const
{
	return " (text . \""+Utils::simpleQuote(wnd_->toPlainText())+"\" )";
}

