#ifndef QTBROWSER_ALWIZARDFACE_HH
#define QTBROWSER_ALWIZARDFACE_HH

#include <QWidget>
#include <QPushButton>
#include <QLabel>

#include "widgets.hh"

class AWizardFace: public QWidget
{
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

    QWidget* addItem(const QString &id, ItemType);
    void setItemText(const QString &id, const QString &value);
    void setItemPixmap(const QString &id, const QString &value);
    QWidget* getItemWidget(const QString &id);
    QWidget* getView();
private:
    QGridLayout* main_layout;
    QVBoxLayout* labels_layout;
    QHBoxLayout* buttons_layout;
    QFrame* buttons_widget;
    QFrame* labels_widget;
    QFrame* view_widget;
    QMap<QString, QAbstractButton*> buttons;
};

class alWizardFaceItem: public alWidget
{
public:
    alWizardFaceItem(const QString& id,const QString& parent, QWidget* wnd);
    ~alWizardFaceItem();

    void setAttr(const QString& name,const QString& value);
    void registerEvent(const QString&);
    QWidget* getWidget() { return wnd_; };
    QWidget* getViewWidget() { return 0; };
    QLayout* getViewLayout() { return 0; };
private:
    QWidget* wnd_;
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
    QLayout* getViewLayout();
};

#endif
