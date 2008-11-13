#ifndef QTBROWSER_AL_RADIO_HH
#define QTBROWSER_AL_RADIO_HH

#include <QRadioButton>

#include "al_widget.hh"

class ARadio: public AWidget<QRadioButton>
{
Q_OBJECT
Q_PROPERTY(QString altgroup READ alteratorGroup WRITE setAlteratorGroup)
public:
    ARadio(QWidget *parent, const Qt::Orientation): AWidget<QRadioButton>(parent) {};
    ~ARadio() {};
};

class alRadio: public alWidgetPre<ARadio>
{
public:
	alRadio(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
		alWidgetPre<ARadio>(attr,WRadio,id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
