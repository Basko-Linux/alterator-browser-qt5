#ifndef QT_BROWSER_HELP_BROWSER_HH
#define QT_BROWSER_HELP_BROWSER_HH

#include <QMenuBar>
#include <QKeyEvent>
#include <QTextBrowser>
#include <QDialogButtonBox>

#include "popup.hh"

class HelpWidget: public Popup
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
#if 0
    virtual void showEvent(QShowEvent*);
    virtual void paintEvent(QPaintEvent*);
#endif
    virtual void showEvent(QShowEvent*);
    virtual void keyPressEvent(QKeyEvent*);

private slots:
    void execLink(const QUrl&);
    void onButtonPressed(QAbstractButton*);

private:
    QGridLayout *gridLayout;
    QTextBrowser *textBrowser;
    QDialogButtonBox *buttonBox;
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
    void onButtonPressed(int);

private:
    HelpWidget *help_widget;
    QString help_url;
    int vscroll_position;
};

#endif
