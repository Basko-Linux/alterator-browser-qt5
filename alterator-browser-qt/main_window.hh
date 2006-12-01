#ifndef QT_BROWSER_MAIN_WINDOW_HH
#define QT_BROWSER_MAIN_WINDOW_HH

#include <QFrame>
#include <QMainWindow>
#include <QGridLayout>
#include <QXmlAttributes>

typedef QMainWindow MainWindow_t;
typedef QGridLayout MainWindowLayout_t;

class MainWindow: public MainWindow_t
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

    bool haveWindowManager();
    void setAttributes(const QXmlAttributes&);

public slots:
    void messageBox();

private:
    bool started;
    bool have_wm;
    bool detect_wm;
    QXmlAttributes xml_attributes;

//    void showEvent(QShowEvent*);
//    void customEvent(QEvent*);

private slots:
    void start();
    void stop();
};
#endif
