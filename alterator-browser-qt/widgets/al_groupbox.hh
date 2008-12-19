#ifndef QTBROWSER_AL_GROUPBOX_HH
#define QTBROWSER_AL_GROUPBOX_HH

#include <QGroupBox>

#include "al_widget.hh"

class AGroupBox: public AWidget<QGroupBox>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    AGroupBox(QWidget *parent, const Qt::Orientation);
    ~AGroupBox();

    void setMyTitle(const QString&);

private:
    QBoxLayout *l;
};

class alGroupBox: public alWidgetPre<AGroupBox>
{
public:
	alGroupBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
