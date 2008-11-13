#ifndef QTBROWSER_AL_SLIDER_HH
#define QTBROWSER_AL_SLIDER_HH

#include <QSlider>

#include "al_widget.hh"

class ASlider: public AWidget<QSlider>
{
Q_OBJECT
Q_PROPERTY(QString altgroup READ alteratorGroup WRITE setAlteratorGroup)
public:
    ASlider(QWidget *parent, const Qt::Orientation);
    ~ASlider();

signals:
    void valueReallyChanged();

private:
    int timeout;
    int timer;
    int old_value;

    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);
    void timerEvent(QTimerEvent*);
    void checkValueReallyChanged();

private slots:
    void onValueChange(int);
};

class alSlider: public alWidgetPre<ASlider>
{
Q_OBJECT
public:
    alSlider(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent);
    void setAttr(const QString& name,const QString& value);
    void registerEvent(const QString&);
    QString postData() const;
};

#endif
