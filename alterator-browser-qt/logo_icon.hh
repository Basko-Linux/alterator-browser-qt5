#ifndef QT_BROWSER_LOGO_ICON_HH
#define QT_BROWSER_LOGO_ICON_HH

#include <QLabel>

class LogoIcon: public QLabel
{
Q_OBJECT
public:
    LogoIcon(QWidget *parent, bool clickable = false);
    ~LogoIcon();

    virtual void mousePressEvent(QMouseEvent*);

Q_SIGNALS:
    void clicked();

private:
    bool clickable;
};

#endif
