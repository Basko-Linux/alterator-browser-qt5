#ifndef QTBROWSER_AL_MAIN_WIDGET_HH
#define QTBROWSER_AL_MAIN_WIDGET_HH

#include <QApplication>

#include "al_widget.hh"
#include "browser.hh"

class AMainWidget: public AWidget<QFrame>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    AMainWidget(QWidget *parent, const Qt::Orientation);
    ~AMainWidget();
};

class alMainWidget: public alWidgetPre<AMainWidget>
{
public:
	alMainWidget(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void popUp() {}
	void popDown()  { browser->quitAppManaged(0); }
};

#endif
