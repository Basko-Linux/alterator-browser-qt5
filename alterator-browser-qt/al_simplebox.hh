#ifndef QTBROWSER_AL_GRIDEBOX_HH
#define QTBROWSER_AL_GRIDEBOX_HH

#include <QWidget>
#include <QGridLayout>

#include "al_widget.hh"

class ASimpleBox: public QWidget
{
public:
    ASimpleBox(QWidget *parent);
    ~ASimpleBox();

    void setColumns(const QString&);
    QWidget* getView();
    void addChild(QWidget *chld);


private:
    QGridLayout *layout_;
    int columns_;
    int current_row;
    int current_column;
    QWidget *view_widget;
};

class alSimpleBox: public alWidgetPre<ASimpleBox>
{
public:
    alSimpleBox(const QString &id,const QString &parent, const QString &columns);
    void setAttr(const QString &name,const QString &value);
    QWidget* getViewWidget();
    QLayout* getViewLayout();
    void addChild(QWidget *chld, alWidget::Type type);
};

#endif
