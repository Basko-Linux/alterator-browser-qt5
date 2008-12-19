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

class ATabPage: public AWidget<QWidget>
{
Q_OBJECT
Q_PROPERTY(QString altgroup READ alteratorGroup WRITE setAlteratorGroup)
public:
    ATabPage(QWidget *parent, const Qt::Orientation): AWidget<QWidget>(parent) {};
    ~ATabPage() {};
};

class alTabBox: public alWidgetPre<ATabWidget>
{
public:
	alTabBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
	Qt::Orientation getOrientation() { return orientation_; };
private:
	Qt::Orientation orientation_;
};

class alTabPage: public alWidgetPre<ATabPage>
{
	alTabBox  *tabbox_;
	int idx_;
public:
	alTabPage(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation);
	void setAttr(const QString& name,const QString& value);
private:
	static alTabBox* getParentTabBox(const QString& parent);
};

#endif
