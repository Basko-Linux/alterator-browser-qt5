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
    ADigitalClock *clock;
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
