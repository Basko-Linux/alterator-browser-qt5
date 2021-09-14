#ifndef QT_BROWSER_POPUP_HH
#define QT_BROWSER_POPUP_HH

#include <QFrame>
#include <QLabel>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QChildEvent>
#include <QEventLoop>

class Popup: public QFrame
{
Q_OBJECT
public:
    Popup(QWidget *parent, bool title = false, bool winexpand = false, bool winclose = false, bool winshrink = false);
    ~Popup();

    void setPopupTitle(const QString&);
    QWidget* view();

Q_SIGNALS:
    void finished(int);
    void accepted();
    void rejected();

public Q_SLOTS:
    virtual void done(int);
    virtual void accept();
    virtual void reject();
    virtual int exec();

protected:
    virtual void keyPressEvent(QKeyEvent*);

private Q_SLOTS:
    void onWinCloseClicked();
    void onWinExpandClicked();
    void onWinShrinkClicked();

private:
    int result_code;
    bool has_title;
    bool has_winclose;
    bool has_winexpand;
    bool has_winshrink;
    QEventLoop *event_loop;
    QVBoxLayout *vlayout;
    QWidget *view_widget;
    QLabel *title_txt;
    QFrame *title_separator;
    QSizePolicy szpol;
};

#endif
