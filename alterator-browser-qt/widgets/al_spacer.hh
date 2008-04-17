#ifndef QTBROWSER_AL_SPACER_HH
#define QTBROWSER_AL_SPACER_HH

#include <QWidget>
#include "al_widget.hh"

class ASpacer: public QWidget
{
public:
    ASpacer(QWidget *parent, const Qt::Orientation): QWidget(parent) {};
    ~ASpacer() {};
};

class alSpacer: public alWidgetPre<ASpacer>
{
public:
	alSpacer(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
//	void setAttr(const QString& name,const QString& value);
//	void registerEvent(const QString&);
//	QString postData() const ;
};

#endif
