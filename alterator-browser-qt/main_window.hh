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
    void changeLanguage(const QString&);

signals:
    void languageChanged();

public slots:
    void start();
    void stop();

protected:
//    virtual void childEvent(QChildEvent*);
    virtual bool event(QEvent*);

private:
    QTranslator *qtranslator;
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
