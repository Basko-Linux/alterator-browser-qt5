
#include <QScrollBar>

#include "main_window.hh"
#include "help_browser.hh"
#include "main_window.hh"
#include "utils.hh"

extern MainWindow *main_window;

HelpWidget::HelpWidget(QWidget *parent):
    QDialog(parent)
{
    ui.setupUi(this);
    //setWindowTitle(tr("Help"));
    //setWindowTitle(QApplication::translate("QDialogButtonBox", "Help", 0, QApplication::UnicodeUTF8));
    setEmptyHelp();

    connect(ui.textBrowser, SIGNAL(anchorClicked(const QUrl&)),
	    ui.textBrowser, SLOT(setSource(const QUrl&)));
}

HelpWidget::~HelpWidget() {}

void HelpWidget::setHelpSource(const QString& url)
{
    if(url.isEmpty())
	setEmptyHelp();
    else
	ui.textBrowser->setSource(url);
}

void HelpWidget::setEmptyHelp()
{
    ui.textBrowser->setHtml( "<br/><br/><br/><br/><center><b>No help available.</b></center>" );
}


void HelpWidget::paintEvent(QPaintEvent* e)
{
    //QDialog::paintEvent(e);
    if( !main_window->haveWindowManager() )
    {
	Utils::widgetCornersRound(this);
    }
}

void HelpWidget::showEvent(QShowEvent *e)
{
    //qDebug("HelpWidget::showEvent");
    //HelpWidget::showEvent(e);
    if( !main_window->haveWindowManager() )
    {
	Utils::fixWmlessPopup(this);
    }
}

int HelpWidget::verticalScrollBarPosition()
{
    return ui.textBrowser->verticalScrollBar()->sliderPosition();
}

void HelpWidget::setVerticalScrollBarPosition(int pos)
{
    ui.textBrowser->verticalScrollBar()->setSliderPosition(pos);
}

// HelpBrowser

HelpBrowser::HelpBrowser(QObject *parent):
    QObject(parent)
{
    help_widget = 0;
    vslider_position = 0;
}

HelpBrowser::~HelpBrowser()
{}

void HelpBrowser::setHelpSource(const QString& url)
{
    if( help_url != url )
	vslider_position = 0;
    help_url = url;
}

int HelpBrowser::exec()
{
    int ret = -1;
    if( !help_widget )
    {
	help_widget = new HelpWidget(QApplication::activeWindow());
	help_widget->setHelpSource(help_url);
	if( vslider_position > 0 )
	    help_widget->setVerticalScrollBarPosition(vslider_position);
	ret = help_widget->exec();
	vslider_position = help_widget->verticalScrollBarPosition();
	delete help_widget;
	help_widget = 0;
    }
    return ret;
}
