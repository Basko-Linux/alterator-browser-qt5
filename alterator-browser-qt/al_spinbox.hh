#ifndef QTBROWSER_AL_SPINBOX_HH
#define QTBROWSER_AL_SPINBOX_HH

#include <QSpinBox>

#include "al_widget.hh"

class alSpinBox: public alWidgetPre<QSpinBox>
{
Q_OBJECT
public:
    alSpinBox(const QString& id, const QString& parent):
	alWidgetPre<QSpinBox>(SpinBox,id,parent)
    {
    }
    void setAttr(const QString& name,const QString& value);
    void registerEvent(const QString&);
    QString postData() const;
};

#endif
