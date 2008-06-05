#ifndef QT_BROWSER_SPLASHSCREEN_HH
#define QT_BROWSER_SPLASHSCREEN_HH

#include <QFrame>
#include <QLineEdit>

class SplashScreen: public QFrame
{
public:
    SplashScreen(QWidget *parent);
    ~SplashScreen();

    void setText(const QString&);

private:
    QLineEdit *label;
};

#endif
