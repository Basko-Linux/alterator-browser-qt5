#ifndef QTBROWSER_AL_WIZARDFACE_HH
#define QTBROWSER_AL_WIZARDFACE_HH

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QSignalMapper>

#include "widgets.hh"

class AWizardFace: public QWidget
{
    Q_OBJECT;
public:
    AWizardFace( QWidget *parent=0, Qt::WFlags f=0 );
    ~AWizardFace();

    enum ItemType
    {
	ItemGeneric,
	ItemQuit,
	ItemHelp,
	ItemApply,
	ItemCancel,
	ItemBackward,
	ItemForward,
	ItemStep,
	ItemSection
    };

    QWidget* addItem(const QString &key, ItemType);
    void setCurrentStep( int );
    void setTitle( const QString &value);
    void setItemText(const QString &key, const QString &value);
    void setItemPixmap(const QString &key, const QString &value);
    void setItemActivity(const QString &key, bool);
    QWidget* getItemWidget(const QString &key);
    QWidget* getView();
    void cleanRequest();
    QString current();

signals:
    void itemSelected();
    void itemSelected(const QString&);

private slots:
    void onSelect(const QString&);

private:
    QString current_;
    QSignalMapper *signal_mapper;
    QGridLayout* main_layout;
    QVBoxLayout* labels_layout;
    QHBoxLayout* buttons_layout;
    QFrame* buttons_widget;
    QFrame* labels_widget;
    QWidget* view_widget;
    QLabel* title;
    QMap<QString, QAbstractButton*> buttons;
    QMap<QString, ItemType> types;

    int newButtonPosition(ItemType);
    int findButtonPosition(ItemType);
    Qt::Alignment newButtonAlignment(ItemType);
    void setButtonIcon(QAbstractButton*, ItemType);
};

class alWizardFace: public alWidgetPre<AWizardFace>
{
private:
    QMap<QString,AWizardFace::ItemType> key2type;
    void addItem(const QString& key, const QString& name, const QString& pixmap);
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
