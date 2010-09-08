#ifndef QTBROWSER_AL_SLIDER_HH
#define QTBROWSER_AL_SLIDER_HH

#include <QSlider>

#include "al_widget.hh"

class ASlider: public AWidget<QSlider>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ASlider(QWidget *parent, const Qt::Orientation);
    ~ASlider();

Q_SIGNALS:
    void valueReallyChanged();

private:
    QTimer *tmr;
    int old_value;

    void focusInEvent(QFocusEvent*);
    void focusOutEvent(QFocusEvent*);

private Q_SLOTS:
    void onValueChange(int);
    void checkValueReallyChanged();
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
