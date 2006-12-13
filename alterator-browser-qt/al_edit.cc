#include <QPainter>

#include "utils.hh"
#include "al_edit.hh"

AEdit::AEdit(QWidget *parent):
    QWidget(parent)
{
    mark = new QLabel("*" ,this);
    mark->hide();
    QPalette palet = mark->palette();
    palet.setBrush(QPalette::Foreground, QColor("red"));
    mark->setPalette(palet);

    edit = new QLineEdit(this);
    edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    connect(edit, SIGNAL(textEdited(const QString&)), this, SIGNAL(textEdited(const QString&)));

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(mark);
    layout->addWidget(edit);
}

AEdit::~AEdit() {}

void AEdit::setText(const QString& txt)
{
    edit->setText(txt);
}

void AEdit::setEchoMode(QLineEdit::EchoMode emode)
{
    edit->setEchoMode(emode);
}

void AEdit::setReadOnly(bool ro)
{
    edit->setReadOnly(ro);
}

QString AEdit::text()
{
    return edit->text();
}


void AEdit::markRequired(bool req)
{
    if(req)
	mark->show();
    else
	mark->hide();
}

// alEdit
alEdit::alEdit(const QString& id,const QString& parent):
		alWidgetPre<AEdit>(Edit,id,parent)
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
	return " (text . \""+Utils::simpleQuote(wnd_->text())+"\" )";
}

void alEdit::markRequired(bool req)
{
    wnd_->markRequired(req);
}
