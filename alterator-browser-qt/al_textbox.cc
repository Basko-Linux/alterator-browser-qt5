
#include "utils.hh"
#include "al_textbox.hh"


ATextEdit::ATextEdit(QWidget *parent):
    QWidget(parent)
{
    mark = new QLabel("*" ,this);
    mark->hide();
    QPalette palet = mark->palette();
    palet.setBrush(QPalette::Foreground, QColor("red"));
    mark->setPalette(palet);

    edit = new QTextEdit(this);
    edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect(edit, SIGNAL(textChanged()), this, SIGNAL(textEdited()));

    layout_ = new QHBoxLayout(this);
    layout_->setMargin(0);
    layout_->setSpacing(0);
    layout_->addWidget(mark);
    layout_->addWidget(edit);
}

ATextEdit::~ATextEdit() {}

void ATextEdit::setText(const QString& txt)
{
    edit->setPlainText(txt);
}

void ATextEdit::append(const QString& txt)
{
    edit->append(txt);
}

void ATextEdit::setReadOnly(bool ro)
{
    edit->setReadOnly(ro);
}

QString ATextEdit::text()
{
    return edit->toPlainText();
}


void ATextEdit::markRequired(bool req)
{
    if(req)
    {
	mark->show();
	layout_->setSpacing(5);
    }
    else
    {
	mark->hide();
	layout_->setSpacing(0);
    }
}


// alTextBox
alTextBox::alTextBox(const QString& id,const QString& parent):
		alWidgetPre<ATextEdit>(TextBox,id,parent)
{
}

void alTextBox::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
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
	return " (text . \""+Utils::simpleQuote(wnd_->text())+"\" )";
}

