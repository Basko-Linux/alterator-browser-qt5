#ifndef QT_BROWSER_MAIN_WINDOW_HH
#define QT_BROWSER_MAIN_WINDOW_HH

#include <QDialog>
#include <QMainWindow>
#include <QGridLayout>
#include <QXmlAttributes>

typedef QMainWindow MainWindow_t;
//typedef QWidget MainWindow_t;
typedef QGridLayout MainWindowLayout_t;

class MainWindow: public MainWindow_t
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

    bool haveWindowManager();
    void setHaveWindowManager(bool);
    void setAttributes(const QXmlAttributes&);
    void setFullScreen(bool);

public slots:
    void messageBox();

private:
    QRect geometry_;
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
