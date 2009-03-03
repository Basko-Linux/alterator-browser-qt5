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

signals:
    void clicked();
private:
    bool clickable;
};

#endif
