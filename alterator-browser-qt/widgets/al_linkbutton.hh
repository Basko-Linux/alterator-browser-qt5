#ifndef QTBROWSER_AL_LINKBUTTON_HH
#define QTBROWSER_AL_LINKBUTTON_HH

#include <QLabel>

#include "al_widget.hh"

class ALinkButton: public AWidget<QWidget>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ALinkButton(QWidget *parent, const Qt::Orientation);
    ~ALinkButton();

    void setText(const QString&);
    void setPixmap(const QPixmap&);

Q_SIGNALS:
    void clicked();

private Q_SLOTS:
    void onLinkActivated(const QString&);

private:
    QLabel *txt_lbl;
    QLabel *pix_lbl;
};

class alLinkButton: public alWidgetPre<ALinkButton>
{
public:
	alLinkButton(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
		alWidgetPre<ALinkButton>(attr,WLinkButton,id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
};

#endif
