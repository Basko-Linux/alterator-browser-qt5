#ifndef QTBROWSER_AL_SPLITBOX_HH
#define QTBROWSER_AL_SPLITBOX_HH

#include <QSplitter>

#include "al_widget.hh"

class ASplitBox: public AWidget<QSplitter>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ASplitBox(QWidget *parent, const Qt::Orientation);
    ~ASplitBox();

    void postAddChild(QWidget *chld);
    void setColumns(const QString &columns);

protected:
	void showEvent(QShowEvent *e);

private:
    QList<int> columns_;
    QList<int> columns_add_;
    int columns_sum;
};

class alSplitBox: public alWidgetPre<ASplitBox>
{
public:
    alSplitBox(const AlteratorRequestActionAttrs &attr, const QString &id,const QString &parent, Qt::Orientation orientation, const QString &columns);

    void setAttr(const QString& name,const QString& value);
    void postAddChild(QWidget *chld, AlteratorWidgetType type, const AlteratorRequestActionAttrs &attr);
};

#endif
