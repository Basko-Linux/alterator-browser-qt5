#ifndef QTBROWSER_AL_SPINBOX_HH
#define QTBROWSER_AL_SPINBOX_HH

#include <QDoubleSpinBox>

#include "al_widget.hh"

class alSpinBox: public alWidgetPre<QDoubleSpinBox>
{
Q_OBJECT
public:
    alSpinBox(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent);
    void setAttr(const QString& name,const QString& value);
    void registerEvent(const QString&);
    QString postData() const;
};

#endif
