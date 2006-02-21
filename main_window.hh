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

private:
    bool started;
    void showEvent(QShowEvent*);

private slots:
    void start();
};
#endif
