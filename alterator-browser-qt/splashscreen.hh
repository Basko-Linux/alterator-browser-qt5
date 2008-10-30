#ifndef QT_BROWSER_SPLASHSCREEN_HH
#define QT_BROWSER_SPLASHSCREEN_HH

#include <QLineEdit>

#include "popup.hh"

class SplashScreen: public Popup
{
public:
    SplashScreen(QWidget *parent);
    ~SplashScreen();

    void setText(const QString&);

private:
    QLineEdit *label;
};

#endif
