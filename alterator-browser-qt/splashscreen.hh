#ifndef QT_BROWSER_SPLASHSCREEN_HH
#define QT_BROWSER_SPLASHSCREEN_HH

#include <QLabel>

class SplashScreen: public QWidget
{
public:
    SplashScreen(QWidget *parent);
    ~SplashScreen();


    void setText(const QString&);

protected:
	void paintEvent(QPaintEvent*);

private:
    QLabel *label;

    void applyMask();
};

#endif

