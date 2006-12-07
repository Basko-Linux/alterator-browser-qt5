#ifndef QT_BROWSER_HELP_BROWSER_HH
#define QT_BROWSER_HELP_BROWSER_HH


#include "ui_help_browser.h"

class HelpBrowser: public QDialog
{
public:
    HelpBrowser(QWidget *parent);
    ~HelpBrowser();

    void setHelpSource(const QString&);

private:
    Ui::HelpBrowserUI ui;
};

#endif
