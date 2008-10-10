
#include <QScrollBar>
#include <QProcess>

#include "browser.hh"
#include "browser.hh"
#include "utils.hh"
#include "a_pixmaps.hh"

#include "help_browser.hh"

HelpWidget::HelpWidget(QWidget *parent):
    QDialog(parent)
{
    vscroll_position = 0;
    ui.setupUi(this);

    menu_bar = new QMenuBar(this);
    menu_bar->hide();
    QMenu *menu_session = new QMenu(tr("Session"), menu_bar);
    QMenu *menu_help = new QMenu(tr("Help"), menu_bar);
    menu_bar->addMenu(menu_session);
    menu_bar->addMenu(menu_help);
    menu_session->addAction(QApplication::style()->standardPixmap(QStyle::SP_DialogCancelButton), tr("Quit"), browser, SLOT(quitAppWarn()));
    menu_help->addAction(getPixmap("logo_16"), tr("About"), browser, SLOT(about()));
    menu_help->addAction(QApplication::style()->standardPixmap(QStyle::SP_TitleBarMenuButton), tr("About Qt"), QApplication::instance(), SLOT(aboutQt()));
    ui.gridLayout->setMenuBar(menu_bar);

    connect(ui.textBrowser, SIGNAL(anchorClicked(const QUrl&)),
	    ui.textBrowser, SLOT(setSource(const QUrl&)));
}

HelpWidget::~HelpWidget() {}

void HelpWidget::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
	case Qt::Key_F1:
	{
	    menu_bar->setVisible(!menu_bar->isVisible());
	    break;
	}
	default:
	    break;
    }
    QDialog::keyPressEvent(e);
}

void HelpWidget::setHelpSource(const QString& url)
{
    if(url.isEmpty())
	setEmptyHelp();
    else
	ui.textBrowser->setSource(url);
}

void HelpWidget::setEmptyHelp()
{
    ui.textBrowser->setHtml( QString("<br/><br/><br/><br/><center><b>%1</b></center>").arg(tr("No help available.")) );
}


void HelpWidget::paintEvent(QPaintEvent* e)
{
    //QDialog::paintEvent(e);
    if( !browser->haveWindowManager() )
    {
	Utils::widgetCornersRound(this);
    }
}

void HelpWidget::showEvent(QShowEvent *e)
{
    //qDebug("HelpWidget::showEvent");
    //HelpWidget::showEvent(e);
    if( !browser->haveWindowManager() )
    {
	Utils::fixWmlessPopup(this);
    }
    if( vscroll_position > 0 )
	ui.textBrowser->verticalScrollBar()->setValue(vscroll_position);
}

int HelpWidget::verticalScrollPosition()
{
    return ui.textBrowser->verticalScrollBar()->value();
}

void HelpWidget::setVerticalScrollPosition(int pos)
{
    vscroll_position = pos;
}

void HelpWidget::execLink(const QUrl &url)
{
    QString prog_command;
    if( url.scheme() == "http" || url.scheme() == "https" || url.scheme() == "ftp" )
    {
	prog_command = getenv("HELP_BROWSER");
	if( prog_command.isEmpty() )
	{
	    qWarning("%s", qPrintable(tr("HELP_BROWSER environment variable is empty")));
	    prog_command = getenv("BROWSER");
	    if( prog_command.isEmpty() )
		qWarning("%s", qPrintable(tr("BROWSER environment variable is empty")));
	}
    }
    else if( url.scheme() == "mailto" )
    {
	prog_command = getenv("MAILER");
	if( prog_command.isEmpty() )
	    qWarning("%s", qPrintable(tr("MAILER environment variable is empty")));
    }
    ui.textBrowser->reload();
    
    if( prog_command.isEmpty() )
    {
	prog_command = "url_handler.sh";
	//qDebug("Set prog_command to %s", qPrintable(prog_command));
    }
    
    QProcess *prog_proc = new QProcess(this);
    QStringList prog_args = prog_command.split(" ", QString::SkipEmptyParts);
    prog_args << url.toString() ;
    QString prog_name = prog_args.takeFirst();
    prog_proc->start(prog_name, prog_args);
}

// HelpBrowser

HelpBrowser::HelpBrowser(QObject *parent):
    QObject(parent)
{
    help_widget = 0;
    vscroll_position = 0;
}

HelpBrowser::~HelpBrowser()
{}

void HelpBrowser::setHelpSource(const QString& url)
{
    if( help_url != url )
	vscroll_position = 0;
    help_url = url;
}

int HelpBrowser::exec()
{
    int ret = -1;
    if( !help_widget )
    {
	help_widget = new HelpWidget(QApplication::activeWindow());
	int w = browser->width()*0.8;
	int h = browser->height()*0.8;
	if( w > 400 && h > 300 )
	    help_widget->resize(browser->width()*0.8, browser->height()*0.8);
	help_widget->setHelpSource(help_url);
	help_widget->setVerticalScrollPosition(vscroll_position);
	ret = help_widget->exec();
	vscroll_position = help_widget->verticalScrollPosition();
	delete help_widget;
	help_widget = 0;
    }
    return ret;
}
