
#include "main_window.hh"
#include "help_browser.hh"
#include "hacks.hh"

HelpBrowser::HelpBrowser(QWidget *parent):
    QDialog(parent)
{
    ui.setupUi(this);
    ui.textBrowser->setHtml( "<br/><br/><br/><br/><center><b>No help available.</b><center>" );

    connect(ui.textBrowser, SIGNAL(anchorClicked(const QUrl&)),
	    ui.textBrowser, SLOT(setSource(const QUrl&)));
}

HelpBrowser::~HelpBrowser() {}

void HelpBrowser::setHelpSource(const QString& url)
{
    ui.textBrowser->setSource(url);
}

void HelpBrowser::paintEvent(QPaintEvent* e)
{
    if( !main_window->haveWindowManager() )
    {
	//HelpBrowser::paintEvent(e);
	widget_corners_round(this);
    }
}

void HelpBrowser::showEvent(QShowEvent *e)
{
    //qDebug("HelpBrowser::showEvent");
    //HelpBrowser::showEvent(e);
    if( !main_window->haveWindowManager() )
    {
	fix_wmless_window(this);
    }
}
