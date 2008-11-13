#ifndef QTBROWSER_AL_CHECKBOX_HH
#define QTBROWSER_AL_CHECKBOX_HH

#include <QCheckBox>

#include "awidget.hh"
#include "al_widget.hh"

class ACheckBox: public AWidget<QCheckBox>
{
Q_OBJECT
Q_PROPERTY(QString altgroup READ alteratorGroup WRITE setAlteratorGroup)
public:
    ACheckBox(QWidget *parent, const Qt::Orientation): AWidget<QCheckBox>(parent) {};
    ~ACheckBox() {};
};

class alCheckBox: public alWidgetPre<ACheckBox>
{
public:
	alCheckBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);

	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
	QString getValue() { return  (wnd_->isChecked()?"true":"false"); }
};

#endif
