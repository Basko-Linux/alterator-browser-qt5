
#include <QScrollBar>
#include <QProcess>
#include <QApplication>
#include <QFileInfo>
#include <QDesktopServices>

#include "browser.hh"
#include "browser.hh"
#include "utils.hh"
#include "a_pixmaps.hh"

#include "utils.hh"
#include "help_browser.hh"

HelpWidget::HelpWidget(QWidget *parent):
    Popup(parent, true, true, true, true)
{
    vscroll_position = 0;
    setPopupTitle(tr("Help"));
    setObjectName("HelpWidget");

    gridLayout = new QGridLayout(view());
    gridLayout->setSpacing(5);
    gridLayout->setMargin(5);
    textBrowser = new QTextBrowser(this);
    textBrowser->setOpenExternalLinks(false);
    textBrowser->setOpenLinks(false);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Close);
    buttonBox->setVisible(false);

    gridLayout->addWidget(textBrowser, 0, 0, 1, 1);
    gridLayout->addWidget(buttonBox, 1, 0, 1, 1);

    menu_bar = new QMenuBar(this);
    //menu_bar->hide();
    QMenu *menu_session = new QMenu(tr("Session"), menu_bar);
    QMenu *menu_help = new QMenu(tr("Help"), menu_bar);
    menu_bar->addMenu(menu_session);
    menu_bar->addAction(tr("Font%1").arg(QStringLiteral("+")), this, SLOT(fontPlus()));
    menu_bar->addAction(tr("Font%1").arg(QStringLiteral("-")), this, SLOT(fontMinus()));
    menu_bar->addMenu(menu_help);
    menu_session->addAction(QApplication::style()->standardIcon(QStyle::SP_DialogCancelButton), tr("Quit"), g_browser, SLOT(quitAppAsk()));
    menu_help->addAction(getPixmap("logo_16"), tr("About"), g_browser, SLOT(about()));
    menu_help->addAction(QApplication::style()->standardIcon(QStyle::SP_TitleBarMenuButton), tr("About Qt"), QApplication::instance(), SLOT(aboutQt()));
    gridLayout->setMenuBar(menu_bar);

    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonPressed(QAbstractButton*)));
    connect(textBrowser, SIGNAL(anchorClicked(const QUrl&)),
	    this, SLOT(execLink(const QUrl&)));
    connect(buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonPressed(QAbstractButton*)));
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
	    Popup::keyPressEvent(e);
    }
}

void HelpWidget::setHelpSource(const QString& url)
{
    if(!url.isEmpty())
    {
	textBrowser->setSource(url);
    }
    else
    {
	textBrowser->setHtml( QString("<br/><br/><br/><br/><center><b>%1</b></center>").arg(tr("No help available.")) );
    }
}

void HelpWidget::showEvent(QShowEvent *e)
{
    if( vscroll_position > 1 )
	textBrowser->verticalScrollBar()->setValue(vscroll_position);
    e->accept();
}

int HelpWidget::verticalScrollPosition()
{
    return textBrowser->verticalScrollBar()->value();
}

void HelpWidget::setVerticalScrollPosition(int pos)
{
    vscroll_position = pos;
}

void HelpWidget::execLink(const QUrl &url)
{
    if( url.scheme() == "http" || url.scheme() == "https" || url.scheme() == "ftp" || url.scheme() == "mailto" )
    {
	Utils::openUrl(url);
    }
    else
	textBrowser->setSource(url);
}

void HelpWidget::onButtonPressed(QAbstractButton *btn)
{
    Q_EMIT finished( buttonBox->standardButton(btn) );
}

void HelpWidget::fontPlus()
{
    fontResize(1);
}

void HelpWidget::fontMinus()
{
    fontResize(-1);
}

void HelpWidget::fontResize(int addon)
{
    QFont fnt = QApplication::font();
    int sz = fnt.pointSize();
    if( sz > 0 ) {
	fnt.setPointSize(sz+addon);
    } else {
	sz = fnt.pixelSize();
	if( sz > 0 ) {
	    fnt.setPixelSize(sz+addon);
	} else {
	    sz = fnt.pointSizeF();
	    if( sz > 0 ) {
		fnt.setPointSizeF(sz+addon);
	    } else {
		// fallback
		fnt.setPointSize(10);
	    }
	}
    }
    QApplication::setFont(fnt);
    qApp->setStyleSheet("");
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
    if(help_widget)
	help_widget->setHelpSource(help_url);
}

int HelpBrowser::exec()
{
    if( !help_widget )
    {
	HelpWidget helpw(g_browser);
	help_widget = &helpw;
	int w = g_browser->width()*0.8;
	int h = g_browser->height()*0.8;
	if( w > 400 && h > 300 )
	    helpw.setMinimumSize(g_browser->width()*0.8, g_browser->height()*0.8);
	helpw.setHelpSource(help_url);
	helpw.setVerticalScrollPosition(vscroll_position);
	helpw.exec();
	g_browser->popupRemove(&helpw);
	help_widget = 0;
    }
    return 0;
}
