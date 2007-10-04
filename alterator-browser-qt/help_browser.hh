#ifndef QT_BROWSER_HELP_BROWSER_HH
#define QT_BROWSER_HELP_BROWSER_HH


#include "ui_help_browser.h"

class HelpWidget: public QDialog
{
Q_OBJECT
public:
    HelpWidget(QWidget *parent);
    ~HelpWidget();

    void setHelpSource(const QString&);
    void setEmptyHelp();
    int verticalScrollPosition();
    void setVerticalScrollPosition(int);

private slots:
    void quit();
    void about();

protected:
    void showEvent(QShowEvent*);
    void paintEvent(QPaintEvent*);


private:
    Ui::HelpBrowserUI ui;
    int vscroll_position;
};

class HelpBrowser: public QObject
{
Q_OBJECT
public:
    HelpBrowser(QObject *parent);
    ~HelpBrowser();

    void setHelpSource(const QString&);

public slots:
    int exec();

private:
    HelpWidget *help_widget;
    QString help_url;
    int vscroll_position;
};

#endif
