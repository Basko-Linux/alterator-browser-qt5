
#include <QScrollBar>
#include <QMenuBar>

#include "main_window.hh"
#include "help_browser.hh"
#include "main_window.hh"
#include "utils.hh"
#include "a_pixmaps.hh"

extern MainWindow *main_window;

HelpWidget::HelpWidget(QWidget *parent):
    QDialog(parent)
{
    vscroll_position = 0;
    ui.setupUi(this);

#if 0
    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *menuSession = new QMenu(tr("Session"), menuBar);
    QMenu *menuHelp = new QMenu(tr("Help"), menuBar);
    menuBar->addMenu(menuSession);
    menuBar->addMenu(menuHelp);
    menuSession->addAction(getPixmap("theme:cancel"), tr("Quit"), this, SLOT(quit()));
    menuHelp->addAction(getPixmap("logo_16"), tr("About"), this, SLOT(about()));
    menuHelp->addAction(QApplication::style()->standardPixmap(QStyle::SP_TitleBarMenuButton), tr("About Qt"), qApp, SLOT(aboutQt()));
    ui.gridLayout->setMenuBar(menuBar);
#endif

    connect(ui.textBrowser, SIGNAL(anchorClicked(const QUrl&)),
	    ui.textBrowser, SLOT(setSource(const QUrl&)));
}

HelpWidget::~HelpWidget() {}

void HelpWidget::quit()
{
    if( QMessageBox::warning(this, tr("Exit alterator"),
	tr("Are you sure to exit alterator?"),
	QMessageBox::Ok|QMessageBox::Cancel, QMessageBox::Cancel) == QMessageBox::Ok )
    {
	QApplication::closeAllWindows();
    }
}

void HelpWidget::about()
{
    QMessageBox::information(this, QMessageBox::tr("About"),
	tr("Alterator Browser"));
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
	int w = main_window->width()*0.8;
	int h = main_window->height()*0.8;
	if( w > 400 && h > 300 )
	    help_widget->resize(main_window->width()*0.8, main_window->height()*0.8);
	help_widget->setHelpSource(help_url);
	help_widget->setVerticalScrollPosition(vscroll_position);
	ret = help_widget->exec();
	vscroll_position = help_widget->verticalScrollPosition();
	delete help_widget;
	help_widget = 0;
    }
    return ret;
}
