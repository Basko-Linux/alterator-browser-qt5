#ifndef QTBROWSER_AL_SPINBOX_HH
#define QTBROWSER_AL_SPINBOX_HH

#include <QDoubleSpinBox>

#include "al_widget.hh"

class ASpinBox: public AWidget<QDoubleSpinBox>
{
Q_OBJECT
Q_PROPERTY(QString altgroup READ alteratorGroup WRITE setAlteratorGroup)
public:
    ASpinBox(QWidget *parent, const Qt::Orientation): AWidget<QDoubleSpinBox>(parent) {};
    ~ASpinBox() {};
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
