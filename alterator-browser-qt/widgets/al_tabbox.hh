#ifndef QTBROWSER_AL_TABBOX_HH
#define QTBROWSER_AL_TABBOX_HH

#include <QTabWidget>

#include "al_widget.hh"

class ATabWidget: public AWidget<QTabWidget>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ATabWidget(QWidget *parent, const Qt::Orientation): AWidget<QTabWidget>(parent) {};
    ~ATabWidget() {};
};

class alTabBox: public alWidgetPre<ATabWidget>
{
public:
	alTabBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
	void postAddChild(QWidget *chld, AlteratorWidgetType type, const AlteratorRequestActionAttrs &attr);
};

#endif
