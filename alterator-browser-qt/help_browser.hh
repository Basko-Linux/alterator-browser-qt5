#ifndef QT_BROWSER_HELP_BROWSER_HH
#define QT_BROWSER_HELP_BROWSER_HH

#include <QMenuBar>
#include <QKeyEvent>

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

protected:
    virtual void showEvent(QShowEvent*);
    virtual void paintEvent(QPaintEvent*);
    virtual void keyPressEvent(QKeyEvent*);


private:
    Ui::HelpBrowserUI ui;
    int vscroll_position;
    QMenuBar *menu_bar;
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
