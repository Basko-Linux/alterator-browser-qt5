#ifndef QTBROWSER_AL_CENTERFACE_HH
#define QTBROWSER_AL_CENTERFACE_HH

#include <QLabel>
#include <QSignalMapper>
#include <QStackedLayout>
#include <QDialogButtonBox>
#include <QToolButton>

#include "enums.hh"
#include "widgets.hh"
#include "flowlayout.hh"

typedef QToolButton ACenterSectionModulesListItem;

class ACenterSectionModulesList: public QWidget
{
Q_OBJECT
public:
    ACenterSectionModulesList(QWidget *parent);
    ~ACenterSectionModulesList();

    void addItem(ACenterSectionModulesListItem*);
    void setItemText(ACenterSectionModulesListItem*, const QString&);
    void setItemIcon(ACenterSectionModulesListItem*, const QIcon&);
    void removeItem(ACenterSectionModulesListItem*);

    int count();
    bool isOwnerOfItem(ACenterSectionModulesListItem*);

signals:
    void itemClicked(ACenterSectionModulesListItem*);

private slots:
    void onItemClicked(QWidget*);

private:
    FlowLayout *lay;
    QList<ACenterSectionModulesListItem*> items;
    QSignalMapper *signal_mapper;
};

class ACenterSection: public QWidget
{
public:
    ACenterSection(QWidget *parent, const QString &title_text = "");
    ~ACenterSection();

    void setPixmap(const QPixmap&);
    void setText(const QString&);
    QString getText();

    ACenterSectionModulesList *getModulesList();
private:
    QLabel *pixmap;
    QLabel *title;
    QFrame *separator;
    ACenterSectionModulesList *modlist;
};

class ACenterFace: public QWidget
{
    Q_OBJECT
public:
    ACenterFace(QWidget *parent, const Qt::Orientation);
    ~ACenterFace();

    void clearActions();
    void addAction(const QString& key, const QString& name, const QString& pixmap);
    void removeAction(const QString &key);
    void setActionText(const QString &key, const QString &value);
    void setActionPixmap(const QString &key, const QString &value);

    void clearModules();
    void addModule(const QString& section_key, const QString& key, const QString& name);
    void removeModule(const QString &key);
    void setModuleText(const QString &key, const QString &value);
    void setModulePixmap(const QString &key, const QString &value);

    void clearSections();
    void addSection(const QString& key, const QString& name, const QString& pixmap);
    void setSectionText(const QString &key, const QString &value);
    void setSectionPixmap(const QString &key, const QString &value);
    void removeSection(const QString& key);

    QString currentActionKey();
    QString currentModuleKey();

    QWidget* getViewWidget();
    QLayout* getViewLayout();

signals:
    void actionSelected();
    void actionSelected(const QString& key);
    void moduleSelected();

private slots:
    void onSelectAction(const QString&);
    void onSelectModule(ACenterSectionModulesListItem*);
    void onOwerviewClicked();

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

    QMap<QString, ACenterSection*> sections;
    QMap<QString, ACenterSectionModulesListItem*> modules;
    QMap<QString, QAbstractButton*> buttons;

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
