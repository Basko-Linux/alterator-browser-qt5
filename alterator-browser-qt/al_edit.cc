#include "utils.hh"
#include "al_edit.hh"

alEdit::alEdit(const QString& id,const QString& parent):
		alWidgetPre<QLineEdit>(Edit,id,parent)
{
}

void alEdit::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("echo" == name) 
		wnd_->setEchoMode(Utils::convertEchoMode(value));
	else if ("alterability" == name)
		wnd_->setReadOnly("false" == value);
	else
		alWidget::setAttr(name,value);
}

void alEdit::registerEvent(const QString& name)
{
	if( "changed" == name )
		connect(wnd_,SIGNAL( textEdited(const QString&) ),SLOT(onChange(const QString&)));
	else if( "return-pressed" == name )
	    connect(wnd_,SIGNAL(returnPressed() ),SLOT(onReturn()));
}


QString alEdit::postData() const
{
	return " (text . \""+simpleQuote(wnd_->text())+"\" )";
}
