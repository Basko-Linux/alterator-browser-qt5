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

    void setOffcet(int);

private:
    int offset;
    double deg_per_hou, deg_per_min, deg_per_sec;
    QPen hpen, mpen, spen;
};

class ATimeEdit: public QWidget
{
Q_OBJECT;
public:
    ATimeEdit(QWidget *parent = 0);
    ~ATimeEdit();

    void setExpanded(bool);
    void setTime(const QString&);
    QString time();

signals:
    void changed();

private:
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
    alTimeEdit(const QString& id,const QString& parent);
    void setAttr(const QString& name,const QString& value);
    void registerEvent(const QString&);
    QString postData() const;
};

#endif
