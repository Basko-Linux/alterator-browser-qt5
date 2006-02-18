#ifndef QT_BROWSER_MAIN_WINDOW_HH
#define QT_BROWSER_MAIN_WINDOW_HH

#include <QFrame>

#include "layout.hh"

#define MainWindow_t QWidget
#define MainWindowLayout_t QGridLayout

class MainWindow: public MainWindow_t
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

private slots:
    void start();
};
#endif
