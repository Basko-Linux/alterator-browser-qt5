#ifndef QT_BROWSER_HELP_BROWSER_HH
#define QT_BROWSER_HELP_BROWSER_HH


#include "ui_help_browser.h"

class HelpBrowser: public QDialog
{
Q_OBJECT
public:
    HelpBrowser(QWidget *parent);
    ~HelpBrowser();

    void setHelpSource(const QString&);
    void setEmptyHelp();

public slots:
    void retranslateUi();

protected:
    void showEvent(QShowEvent*);
    void paintEvent(QPaintEvent*);

private:
    Ui::HelpBrowserUI ui;
};

#endif
