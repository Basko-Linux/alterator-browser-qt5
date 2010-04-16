#ifndef QTBROWSER_AL_GRIDEBOX_HH
#define QTBROWSER_AL_GRIDEBOX_HH

#include <QWidget>
#include <QGridLayout>

#include "al_widget.hh"

class AGridBox: public AWidget<QWidget>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    AGridBox(QWidget *parent, const Qt::Orientation);
    ~AGridBox();

    void setColumns(const QString&);
    void postAddChild(QWidget *chld, int rowspan, int colspan);
    QLayout* getViewLayout();

    void setExpanded(bool);

private:
    QGridLayout *layout_;
    int columns_;
    int current_row;
    int current_column;
    QList< QPair<int,int> > occuped;
    QVBoxLayout *layout_main;
};

class alGridBox: public alWidgetPre<AGridBox>
{
public:
    alGridBox(const AlteratorRequestActionAttrs &attr, const QString &id,const QString &parent);
    void setAttr(const QString &name,const QString &value);
    QLayout* getViewLayout();
    void postAddChild(QWidget *chld, AlteratorWidgetType type, const AlteratorRequestActionAttrs &attr);
};

#endif
