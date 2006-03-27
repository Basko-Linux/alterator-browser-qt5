#ifndef QT_BROWSER_MAIN_WINDOW_HH
#define QT_BROWSER_MAIN_WINDOW_HH

#include <QFrame>
#include <QMainWindow>
#include <QGridLayout>

#include "layout.hh"

typedef QMainWindow MainWindow_t;
typedef QGridLayout MainWindowLayout_t;

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
    void stop();
};
#endif
