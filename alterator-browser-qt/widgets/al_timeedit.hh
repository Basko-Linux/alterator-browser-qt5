#ifndef QTBROWSER_AL_TIMEEDIT_HH
#define QTBROWSER_AL_TIMEEDIT_HH

#include <QLCDNumber>
#include <QPen>
#include <QTimeEdit>

#include "al_widget.hh"

class AAnalogClock: public QWidget
{
public:
    AAnalogClock(QWidget *parent = 0);
    ~AAnalogClock();

    virtual void paintEvent(QPaintEvent*);

    void setOffset(int);
    void start();
    void stop();

protected:
    void timerEvent(QTimerEvent*);

private:
    QPixmap bg;
    int tmr_id;
    int offset;
    double deg_per_hou, deg_per_min, deg_per_sec;
    QPen hpen, mpen, spen;
};

class ATimeEdit: public QWidget
{
Q_OBJECT
public:
    ATimeEdit(QWidget *parent = 0);
    ~ATimeEdit();

    void setExpanded(bool);
    void setTime(const QString&);
    QString time();
    void start();
    void stop();

signals:
    void changed();

protected:
    void timerEvent(QTimerEvent*);

private:
    int tmr_id;
    AAnalogClock *clock;
    QTimeEdit *time_edit;
    QVBoxLayout *lay;
    int offset;

private slots:
    void showTime();
    void onChange(const QTime&);
};


class alTimeEdit: public alWidgetPre<ATimeEdit>
{
public:
    alTimeEdit(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
    void setAttr(const QString& name,const QString& value);
    void registerEvent(const QString&);
    QString postData() const;
};

#endif
