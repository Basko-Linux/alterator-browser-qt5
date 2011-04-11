#ifndef QTBROWSER_AL_CENTERFACE_HH
#define QTBROWSER_AL_CENTERFACE_HH

#include <QLabel>
#include <QSignalMapper>
#include <QStackedLayout>
#include <QDialogButtonBox>
#include <QToolButton>

#include "flowlayout.h"
#include "enums.hh"
#include "widgets.hh"
#include "awidget.hh"

class ACSListItem: public QToolButton
{
Q_OBJECT
public:
    enum ModuleType {Alterator, External};
    ACSListItem(ModuleType = Alterator, QWidget *parent = 0);
    ~ACSListItem();

    ModuleType moduleType();
    void setCommand(const QString&);
    QString command();

private Q_SLOTS:
    void onClicked();

private:
    ModuleType m_type;
    QString m_command;
};

class ACenterSectionModulesList: public QWidget
{
Q_OBJECT
public:
    ACenterSectionModulesList(QWidget *parent);
    ~ACenterSectionModulesList();

    void addItem(ACSListItem*);
    void setItemText(ACSListItem*, const QString&);
    void setItemIcon(ACSListItem*, const QIcon&);
    void removeItem(ACSListItem*);

    int count();
    bool isOwnerOfItem(ACSListItem*);
    QList<ACSListItem*> getItems();

Q_SIGNALS:
    void itemClicked(ACSListItem*);

private Q_SLOTS:
    void onItemClicked(QWidget*);

private:
    FlowLayout *lay;
    QList<ACSListItem*> items;
    QSignalMapper *signal_mapper;
};

class ACenterSection: public QWidget
{
public:
    ACenterSection(QWidget *parent, const QString &title_text = "");
    ~ACenterSection();

    void setPixmap(const QPixmap&);
    void setText(const QString&);
    void setDesc(const QString&);

    QString text();

    ACenterSectionModulesList *getModulesList();
private:
    QLabel *pixmap;
    QLabel *title;
    QLabel *desc;
    QFrame *separator;
    ACenterSectionModulesList *modlist;
};

class ACenterFace: public AWidget<QFrame>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ACenterFace(QWidget *parent, const Qt::Orientation);
    ~ACenterFace();

    void setHelpSource(const QString&);
    void sortTabOrder();

    void clearActions();
    void addAction(const QString& key, const QString& name, const QString& pixmap);
    void removeAction(const QString &key);
    void setActionText(const QString &key, const QString &value);
    void setActionPixmap(const QString &key, const QString &value);

    void clearModules();
    void addExtModules();
    void addModule(const QString& section_key, const QString& key, const QString& name, const QString& comment = QString(), const QString& command = QString());
    void removeModule(const QString &key);
    void setModuleText(const QString &key, const QString &value);
    void setModulePixmap(const QString &key, const QString &value);

    void clearSections();
    void addSection(const QString& key, const QString& name, const QString& desc, const QString& pixmap);
    void setSectionText(const QString &key, const QString &value);
    void setSectionDesc(const QString &key, const QString &value);
    void setSectionPixmap(const QString &key, const QString &value);
    void removeSection(const QString& key);

    QString currentActionKey();
    QString currentModuleKey();

    QWidget* getViewWidget();
    QLayout* getViewLayout();

Q_SIGNALS:
    void actionSelected(const QString& key);

private Q_SLOTS:
    void onSelectAction(const QString&);
    void onSelectModule(ACSListItem*);
    void onOwerviewClicked();
    void onExpertModeToggled(bool);

private:
    QSignalMapper *action_signal_mapper;
    QSignalMapper *module_signal_mapper;
    QString current_module_key;
    QString current_action_key;
    QWidget* module_widget;
    QWidget* view_widget;
    QWidget* sections_widget;
    QWidget* sections_view_widget;
    QVBoxLayout* sections_view_layout;
    QVBoxLayout* module_layout;
    QBoxLayout* view_layout;
    QStackedLayout* stacked_layout;
    QDialogButtonBox* buttonbox;
    QToolButton *owerview_btn;
    QString help_source;
    QString help_source_module;

    QHash<QString, ACenterSection*> sections;
    QList<ACenterSection*> sections_list;
    QHash<QString, ACSListItem*> modules;
    QHash<QString, QAbstractButton*> buttons;

    void addAction(const QString &key, UserActionType);
    QPixmap defaultActionIcon(UserActionType);
};

class alCenterFace: public alWidgetPre<ACenterFace>
{
public:
    alCenterFace(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
    ~alCenterFace();
    void setAttr(const QString& name,const QString& value);
    void registerEvent(const QString&);
    QString postData() const ;
    QWidget* getViewWidget();
    QLayout* getViewLayout();
};

#endif
