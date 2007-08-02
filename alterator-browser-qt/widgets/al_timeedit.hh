#ifndef QTBROWSER_AL_TIMEEDIT_HH
#define QTBROWSER_AL_TIMEEDIT_HH

#include <QLCDNumber>
#include <QPen>
#include <QTimeEdit>

#include "al_widget.hh"

class TimeEditFocusEventHandler: public QObject
{
Q_OBJECT
public:
    TimeEditFocusEventHandler(QObject*);
    ~TimeEditFocusEventHandler();

signals:
    void focusIn();
    void focusOut();

protected:
    bool eventFilter(QObject*, QEvent*);
};

class AnalogClock: public QWidget
{
public:
    AnalogClock(QWidget *parent = 0);
    ~AnalogClock();

    virtual void paintEvent(QPaintEvent*);

    void setTime(const QTime&);
    void setOffset(int);
    void start();
    void stop();

protected:
    void timerEvent(QTimerEvent*);
    void resizeEvent(QResizeEvent*);
    virtual bool event(QEvent*);

private:
    int tmr_id;
    int offset;
    QTime last_time;
    QPen hou_pen, min_pen, sec_pen, round_pen, hou_mrk_pen, min_mrk_pen;
    QColor bg_color;
    int clock_width, clock_height;

    void setup();
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

public slots:
    void start();
    void stop();

signals:
    void changed();


protected:
    void timerEvent(QTimerEvent*);

private:
    int tmr_id;
    AnalogClock *clock;
    QTimeEdit *time_edit;
    QVBoxLayout *lay;
    int offset;
    TimeEditFocusEventHandler *time_edit_focus;

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
