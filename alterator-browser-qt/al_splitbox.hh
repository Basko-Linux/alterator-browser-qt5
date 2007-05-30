#ifndef QTBROWSER_AL_SPLITBOX_HH
#define QTBROWSER_AL_SPLITBOX_HH

#include <QSplitter>

#include "al_widget.hh"

class ASplitBox: public QSplitter
{
public:
    ASplitBox(QWidget *parent);
    ~ASplitBox();

    void addChild(QWidget *chld);


private:
};

class alSplitBox: public alWidgetPre<ASplitBox>
{
public:
    alSplitBox(const QString &id,const QString &parent, Qt::Orientation orientation);
    void addChild(QWidget *chld, alWidget::Type type);
};

#endif
