#ifndef QTBROWSER_AL_TIMEEDIT_HH
#define QTBROWSER_AL_TIMEEDIT_HH

#include <QLCDNumber>
#include <QTimeEdit>

#include "al_widget.hh"

class ADigitalClock : public QLCDNumber
{
Q_OBJECT;
public:
    ADigitalClock(QWidget *parent = 0);
    ~ADigitalClock();

public slots:
    void setTime(const QTime&);

private slots:
    void showTime();

private:
    QTime own_time;
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

private slots:
    void onChange();

private:
    ADigitalClock *clock;
    QTimeEdit *time_edit;
    QVBoxLayout *lay;
    QTime own_time;
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
