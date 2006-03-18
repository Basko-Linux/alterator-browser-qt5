#ifndef QTBROWSER_ALWIZARDFACE_HH
#define QTBROWSER_ALWIZARDFACE_HH

#include <QWidget>

#include "widgets.hh"

class AWizardFace: public QWidget
{
    Q_OBJECT
public:
    AWizardFace( QWidget *parent=0, Qt::WFlags f=0 );
    ~AWizardFace();

    enum ItemType
    {
	ButtonGeneric,
	ButtonHelp,
	ButtonApply,
	ButtonCancel,
	ButtonBackward,
	ButtonForward,
	LabelGeneric,
	LabelSection
    };

    void addItem(const QString &id, ItemType, const QString &text);
    QString lastClickedItem();
    QWidget* viewWidget();
private:
    QGridLayout* main_layout;
    QVBoxLayout* labels_layout;
    QHBoxLayout* buttons_layout;
    QFrame* view;
};


class alWizardFace: public alWidgetPre<AWizardFace>
{
public:
	alWizardFace(const QString& id,const QString& parent);
	~alWizardFace();
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
	QWidget* getViewWidget();
	QLayout* getLayout();
};

#endif
