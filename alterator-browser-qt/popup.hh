#ifndef QT_BROWSER_POPUP_HH
#define QT_BROWSER_POPUP_HH

#include <QDialog>
#include <QKeyEvent>

class Popup: public QDialog
{
Q_OBJECT
public:
    Popup(QWidget *parent);
    ~Popup();

protected:
    virtual void keyPressEvent(QKeyEvent*);

};

#endif
