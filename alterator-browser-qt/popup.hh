#ifndef QT_BROWSER_POPUP_HH
#define QT_BROWSER_POPUP_HH

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QChildEvent>

class Popup: public QFrame
{
Q_OBJECT
public:
    Popup(QWidget *parent, bool title = false, bool winexpand = false, bool winclose = false, bool winshrink = false);
    ~Popup();

    void setPopupTitle(const QString&);
    QWidget* view();

signals:
    void finished(int);
    void accepted();
    void rejected();

public slots:
    virtual void done(int);
    virtual int exec();

protected:
    virtual void keyPressEvent(QKeyEvent*);

private slots:
    void onWinCloseClicked();
    void onWinExpandClicked();
    void onWinShrinkClicked();

private:
    bool has_title;
    bool has_winclose;
    bool has_winexpand;
    bool has_winshrink;
    QVBoxLayout *vlayout;
    QWidget *view_widget;
    QLabel *title_txt;
    QFrame *title_separator;
    QSizePolicy szpol;
};

#endif