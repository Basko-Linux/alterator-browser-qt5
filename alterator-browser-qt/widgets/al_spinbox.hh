#ifndef QTBROWSER_AL_SPINBOX_HH
#define QTBROWSER_AL_SPINBOX_HH

#include <QSpinBox>

#include "al_widget.hh"

class ASpinBox: public AWidget<QWidget>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ASpinBox(QWidget *parent, const Qt::Orientation);
    ~ASpinBox();

    void setValue(int);
    void setMinimum(int);
    void setMaximum(int);
    void setStep(int);

    void setSliderVisibility(bool);
    int value();

signals:
    void editingFinished();

private slots:
    void onUserSliderRelease();
    void onUserSpinBoxRelease();
    void onSliderValueChange(int);
    void onSpinBoxValueChange(int);

private:
    QBoxLayout *blayout;
    QSlider *slider;
    QSpinBox *spinbox;
    bool slider_own_changes;
    bool spinbox_own_changes;
};

class alSpinBox: public alWidgetPre<ASpinBox>
{
Q_OBJECT
public:
    alSpinBox(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent);
    void setAttr(const QString& name,const QString& value);
    void registerEvent(const QString&);
    QString postData() const;
};

#endif
