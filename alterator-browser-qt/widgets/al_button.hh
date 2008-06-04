#ifndef QTBROWSER_AL_BUTTON_HH
#define QTBROWSER_AL_BUTTON_HH

#include <QLabel>

#include "al_widget.hh"

class AButton: public QWidget
{
Q_OBJECT
public:
    AButton(QWidget *parent, const Qt::Orientation);
    ~AButton();

    void setText(const QString&);
    void setPixmap(const QPixmap&);

signals:
    void clicked();

private slots:
    void onLinkActivated(const QString&);

private:
    QLabel *txt_lbl;
    QLabel *pix_lbl;
};

class alButton: public alWidgetPre<AButton>
{
public:
	alButton(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
		alWidgetPre<AButton>(attr,WButton,id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
};

#endif
