
#include <QFileInfo>
#include <QProcess>
#include <QDesktopServices>

#include "utils.hh"
#include "al_textbox.hh"

ATextBox::ATextBox(QWidget *parent, const Qt::Orientation):
    AWidget<QWidget>(parent)
{
    mark = new QLabel(QStringLiteral("*") ,this);
    mark->hide();
    QPalette palet = mark->palette();
    palet.setBrush(QPalette::WindowText, QColor("red"));
    mark->setPalette(palet);

    edit = new QTextBrowser(this);
    edit->setReadOnly(false);
    edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    edit->setOpenExternalLinks(false);
    edit->setOpenLinks(false);
    connect( edit, SIGNAL( anchorClicked(const QUrl&) ), this, SLOT( execLink( const QUrl& ) ) );
    connect(edit, SIGNAL(textChanged()), this, SIGNAL(textEdited()));

    layout_ = new QHBoxLayout(this);
    layout_->setContentsMargins(0,0,0,0);
    layout_->setSpacing(0);
    layout_->addWidget(mark);
    layout_->addWidget(edit);

    setFocusProxy(edit);
}

ATextBox::~ATextBox() {}

void ATextBox::execLink(const QUrl &url)
{
    if( url.scheme() == QStringLiteral("http") || url.scheme() == QStringLiteral("https") || url.scheme() == QStringLiteral("ftp") || url.scheme() == QStringLiteral("mailto") )
    {
	Utils::openUrl(url);
    }
    else
	edit->setSource(url);
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

void ATextBox::showEvent(QShowEvent *e)
{
    edit->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    edit->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    e->accept();
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
	if (QStringLiteral("url") == name)
		wnd_->setUrl(value);
	else if (QStringLiteral("text") == name)
		wnd_->setText(value);
	else if (QStringLiteral("value") == name)
		wnd_->setText(value);
	else if (QStringLiteral("text-append") == name)
		wnd_->append(value);
	else if (QStringLiteral("append-text") == name)
		wnd_->append(value);
	else if (QStringLiteral("alterability") == name)
		wnd_->setReadOnly(QStringLiteral("false") == value);
	else
		alWidget::setAttr(name,value);
}

void alTextBox::registerEvent(const QString& name)
{
	if (QStringLiteral("changed") == name)
		connect(wnd_,SIGNAL( textChanged(const QString&) ),SLOT(onChange(const QString&)));
}

QString alTextBox::postData() const
{
    QString ret;
    if( ! wnd_->isReadOnly() )
    {
	ret.append(QStringLiteral(" (text . \"%1\" )").arg(Utils::simpleQuote(wnd_->text())));
	ret.append(QStringLiteral(" (value . \"%1\" )").arg(Utils::simpleQuote(wnd_->text())));
    }
    ret.append(QStringLiteral(" (url . \"%1\" )").arg(wnd_->url()));
    return ret;
}

