#ifndef QTBROWSER_AL_BOX_HH
#define QTBROWSER_AL_BOX_HH

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "awidget.hh"
#include "al_widget.hh"

class ABox: public AWidget<QWidget>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ABox(QWidget *parent, const Qt::Orientation);
    ~ABox();

    void setMyTitle(const QString&);

private:
    QBoxLayout *l;
};

class alBox: public alWidgetPre<ABox>
{
public:
	alBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
};

#endif
