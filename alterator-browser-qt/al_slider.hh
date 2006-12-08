#ifndef QTBROWSER_AL_SLIDER_HH
#define QTBROWSER_AL_SLIDER_HH

#include <QSlider>

#include "al_widget.hh"

class ASlider: public QSlider
{
Q_OBJECT
public:
    ASlider(QWidget *parent);
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
    alSlider(const QString& id, const QString& parent):
	alWidgetPre<ASlider>(Slider,id,parent)
    {
	wnd_->setOrientation( Qt::Horizontal );
	wnd_->setTickPosition( QSlider::TicksBothSides );
    }
    void setAttr(const QString& name,const QString& value);
    void registerEvent(const QString&);
    QString postData() const;
};

#endif
