#ifndef QTBROWSER_AL_GRIDEBOX_HH
#define QTBROWSER_AL_GRIDEBOX_HH

#include <QWidget>
#include <QGridLayout>

#include "al_widget.hh"

class AGridBox: public QWidget
{
public:
    AGridBox(QWidget *parent);
    ~AGridBox();

    void setColumns(const QString&);
    void addChild(QWidget *chld, int rowspan, int colspan);
    QLayout* getViewLayout();


private:
    QGridLayout *layout_;
    int columns_;
    int current_row;
    int current_column;
    QList< QPair<int,int> > occuped;
};

class alGridBox: public alWidgetPre<AGridBox>
{
public:
    alGridBox(const AlteratorRequestActionAttrs &attr, const QString &id,const QString &parent, const QString &columns);
    void setAttr(const QString &name,const QString &value);
    QLayout* getViewLayout();
    void addChild(QWidget *chld, AlteratorWidgetType type, const AlteratorRequestActionAttrs &attr);
};

#endif