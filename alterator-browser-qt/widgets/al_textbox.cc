
#include "utils.hh"
#include "al_textbox.hh"

ATextBox::ATextBox(QWidget *parent, const Qt::Orientation):
    AWidget<QWidget>(parent)
{
    mark = new QLabel("*" ,this);
    mark->hide();
    QPalette palet = mark->palette();
    palet.setBrush(QPalette::Foreground, QColor("red"));
    mark->setPalette(palet);

    edit = new QTextBrowser(this);
    edit->setReadOnly(false);
    edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    connect( edit, SIGNAL( anchorClicked(const QUrl&) ), this, SLOT( execLink( const QUrl& ) ) );
    connect(edit, SIGNAL(textChanged()), this, SIGNAL(textEdited()));

    layout_ = new QHBoxLayout(this);
    layout_->setMargin(0);
    layout_->setSpacing(0);
    layout_->addWidget(mark);
    layout_->addWidget(edit);

    setFocusProxy(edit);
}

ATextBox::~ATextBox() {}

void ATextBox::execLink(const QUrl&)
{
    edit->reload();
}

void ATextBox::setText(const QString& txt)
{
    edit->setText(txt);
}

void ATextBox::append(const QString& txt)
{
    edit->append(txt);
}

void ATextBox::setReadOnly(bool ro)
{
    edit->setReadOnly(ro);
}

bool ATextBox::isReadOnly()
{
    return edit->isReadOnly();
}

QString ATextBox::text()
{
    return edit->toPlainText();
}


void ATextBox::markRequired(bool req)
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

void ATextBox::showEvent(QShowEvent*)
{
    edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
}

void ATextBox::setUrl(const QString& url)
{
    edit->setSource(QUrl(url,QUrl::StrictMode));
}

QString ATextBox::url()
{
    return edit->source().toString();
}

// alTextBox
alTextBox::alTextBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
		alWidgetPre<ATextBox>(attr,WTextBox,id,parent)
{
}

void alTextBox::setAttr(const QString& name,const QString& value)
{
	if ("url" == name)
		wnd_->setUrl(value);
	else if ("text" == name)
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
    QString ret;
    if( ! wnd_->isReadOnly() )
	ret.append(QString(" (text . \"%1\" )").arg(Utils::simpleQuote(wnd_->text())));
    ret.append(QString(" (url . \"%1\" )").arg(wnd_->url()));
    return ret;
}

