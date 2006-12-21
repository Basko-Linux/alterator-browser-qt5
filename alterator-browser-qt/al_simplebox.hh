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
    enum LayoutType
    {
	LayoutUnknown,
	LayoutColumns,
	LayoutRows
    };

    void appendRow();
    void setColumns(const QString&);

protected:
    virtual void childEvent(QChildEvent*);

private:
    LayoutType layout_type;
    QGridLayout *layout_;
    int columns_;
    int current_row;
    int current_column;
};

class alSimpleBox: public alWidgetPre<ASimpleBox>
{
public:
	alSimpleBox(const QString &id,const QString &parent);
	void setAttr(const QString &name,const QString &value);
};

#endif
