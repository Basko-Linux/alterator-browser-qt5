#ifndef QTBROWSER_AL_BOX_HH
#define QTBROWSER_AL_BOX_HH

#include <QHBoxLayout>
#include <QVBoxLayout>

#include "al_widget.hh"

class ABox: public QWidget
{
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
