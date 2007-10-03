
#include "main_window.hh"
#include "help_browser.hh"
#include "main_window.hh"
#include "utils.hh"

extern MainWindow *main_window;

HelpBrowser::HelpBrowser(QWidget *parent):
    QDialog(parent)
{
    ui.setupUi(this);
    //setWindowTitle(tr("Help"));
    //setWindowTitle(QApplication::translate("QDialogButtonBox", "Help", 0, QApplication::UnicodeUTF8));
    setEmptyHelp();

    connect(ui.textBrowser, SIGNAL(anchorClicked(const QUrl&)),
	    ui.textBrowser, SLOT(setSource(const QUrl&)));
    connect(parent, SIGNAL(languageChanged()), this, SLOT(retranslateUi()));
}

HelpBrowser::~HelpBrowser() {}

void HelpBrowser::setHelpSource(const QString& url)
{
    if(url.isEmpty())
	setEmptyHelp();
    else
	ui.textBrowser->setSource(url);
}

void HelpBrowser::setEmptyHelp()
{
    ui.textBrowser->setHtml( "<br/><br/><br/><br/><center><b>No help available.</b></center>" );
}


void HelpBrowser::paintEvent(QPaintEvent* e)
{
    //QDialog::paintEvent(e);
    if( !main_window->haveWindowManager() )
    {
	Utils::widgetCornersRound(this);
    }
}

void HelpBrowser::showEvent(QShowEvent *e)
{
    //qDebug("HelpBrowser::showEvent");
    //HelpBrowser::showEvent(e);
    if( !main_window->haveWindowManager() )
    {
	Utils::fixWmlessPopup(this);
    }
}

void HelpBrowser::retranslateUi()
{
    ui.retranslateUi(this);
    setWindowTitle(QApplication::translate("QDialogButtonBox", "Help", 0, QApplication::UnicodeUTF8));
    ui.buttonBox->setStandardButtons( ui.buttonBox->standardButtons() );
}
