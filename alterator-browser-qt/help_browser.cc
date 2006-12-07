
#include "main_window.hh"
#include "help_browser.hh"

HelpBrowser::HelpBrowser(QWidget *parent):
    QDialog(parent)
{
    ui.setupUi(this);
    connect(ui.textBrowser, SIGNAL(anchorClicked(const QUrl&)),
	    ui.textBrowser, SLOT(setSource(const QUrl&)));
}

HelpBrowser::~HelpBrowser() {}

void HelpBrowser::setHelpSource(const QString& url)
{
    ui.textBrowser->setSource(url);
}
