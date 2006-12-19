#ifndef QT_BROWSER_MAIN_WINDOW_HH
#define QT_BROWSER_MAIN_WINDOW_HH

#include <QDialog>
#include <QMainWindow>
#include <QGridLayout>
#include <QEvent>

#include "help_browser.hh"

typedef QMainWindow MainWindow_t;

class MainWindow: public MainWindow_t
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

    bool haveWindowManager();
    void setFullScreen(bool);
    void setHelpSource(const QString&);

public slots:
    void start();
    void stop();

protected:
    virtual bool event(QEvent*);

private:
    QRect geometry_;
    bool started;
    bool have_wm;
    bool detect_wm;
    HelpBrowser *help_browser;

    void keyPressEvent(QKeyEvent*);
//    void showEvent(QShowEvent*);
//    void customEvent(QEvent*);

};
#endif
