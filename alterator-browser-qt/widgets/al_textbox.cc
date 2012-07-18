#include <pwd.h>
#include <QFileInfo>
#include <QProcess>
#include <QDesktopServices>

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
    edit->setOpenExternalLinks(false);
    edit->setOpenLinks(false);
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

void ATextBox::execLink(const QUrl &url)
{
    if( url.scheme() == "http" || url.scheme() == "https" || url.scheme() == "ftp" || url.scheme() == "mailto" )
    {
	int loginuid = 0;
	if( getuid() == 0 )
	{
	    int pid = getpid();
	    QString proc_path = QString("/proc/%1/loginuid").arg(pid);
	    if( QFileInfo(proc_path).exists() )
	    {
		QFile proc_file(proc_path);
		QString proc_content;
		if( proc_file.open(QIODevice::ReadOnly) )
		    proc_content = proc_file.readLine().trimmed();
		if( !proc_content.isEmpty() )
		{
		    bool ok;
		    loginuid = proc_content.toInt(&ok, 10);
		    if( !ok )
			loginuid = 0;
		}
	    }
	}
	if( loginuid > 0 )
	{
	    struct passwd *pwd = getpwuid(loginuid);
	    QProcess::startDetached("su", QStringList() << "-l" << "-c" << QString("xdg-open \'").append(url.toString()).append("\'") << pwd->pw_name );
	} else {
	    QDesktopServices::openUrl(url);
	}
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
	else if ("value" == name)
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
    {
	ret.append(QString(" (text . \"%1\" )").arg(Utils::simpleQuote(wnd_->text())));
	ret.append(QString(" (value . \"%1\" )").arg(Utils::simpleQuote(wnd_->text())));
    }
    ret.append(QString(" (url . \"%1\" )").arg(wnd_->url()));
    return ret;
}

