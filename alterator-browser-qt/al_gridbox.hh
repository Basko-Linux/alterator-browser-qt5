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
    void addChild(QWidget *chld);
    QLayout* getViewLayout();


private:
    QGridLayout *layout_;
    int columns_;
    int current_row;
    int current_column;
};

class alGridBox: public alWidgetPre<AGridBox>
{
public:
    alGridBox(const QString &id,const QString &parent, const QString &columns);
    void setAttr(const QString &name,const QString &value);
    QLayout* getViewLayout();
    void addChild(QWidget *chld, alWidget::Type type);
};

#endif
