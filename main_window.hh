#ifndef QT_BROWSER_MAIN_WINDOW_HH
#define QT_BROWSER_MAIN_WINDOW_HH

#include <QFrame>
#include <QMainWindow>
#define MainWindow_t QMainWindow
#define MainWindowLayout_t QGridLayout

#include "layout.hh"

class MainWindow: public MainWindow_t
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

    bool haveWindowManager();

private:
    bool started;
    bool have_wm;
    void showEvent(QShowEvent*);

private slots:
    void start();

};
#endif