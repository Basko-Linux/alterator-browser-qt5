#ifndef QTBROWSER_AL_PROGRESSBAR_HH
#define QTBROWSER_AL_PROGRESSBAR_HH

#include <QProgressBar>

#include "al_widget.hh"

class AProgressBar: public AWidget<QProgressBar>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    AProgressBar(QWidget *parent, const Qt::Orientation): AWidget<QProgressBar>(parent) {};
    ~AProgressBar() {};
};

class alProgressBar: public alWidgetPre<AProgressBar>
{
public:
    alProgressBar(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
protected:
    void setAttr(const QString& name,const QString& value);
};

#endif
