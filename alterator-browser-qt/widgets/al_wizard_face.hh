#ifndef QTBROWSER_AL_WIZARDFACE_HH
#define QTBROWSER_AL_WIZARDFACE_HH

#include <QPushButton>
#include <QLabel>
#include <QSignalMapper>
#include <QMenu>
#include <QScrollArea>

#include "enums.hh"
#include "widgets.hh"

class AWizardFaceStepList: public QList< QPair<QString, QString> >
{
public:
    AWizardFaceStepList() {};
    ~AWizardFaceStepList() {};
};

class AWizardFace: public AWidget<QFrame>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    AWizardFace(QWidget *parent, const Qt::Orientation);
    ~AWizardFace();

    void addAction(const QString& key, const QString& name, const QString& pixmap);
    void removeAction(const QString &key);
    void clearActions();
    void setActionText(const QString &key, const QString &value);
    void setActionPixmap(const QString &key, const QString &value);
    void setActionActivity(const QString &key, bool);

    void addStep(const QString& name, const QString& pixmap);
    void removeStep(int);
    void clearSteps();
    void setStepText(int, const QString &value);
    void setStepPixmap(int, const QString &value);
    void setStepActivity(int, bool);
    void setCurrentStep( int );

    void setTitle( const QString &value);
    QWidget* getViewWidget();
    void cleanRequest();
    QString currentAction();
    int currentStep();
    bool onEnter();

signals:
    void stepSelected();
    void actionSelected();
    void blockingActionSelected(const AlteratorRequestFlags);
// private
    void actionSelected(const QString&);

private slots:
    void onSelectAction(const QString&);
    void onSelectStep(QListWidgetItem*);

protected:
    void keyPressEvent(QKeyEvent*);
    bool eventFilter(QObject*, QEvent*);

private:
    AWizardFaceStepList steplist;
    int current_step;
    QString current_action;
    QSignalMapper *action_signal_mapper;
    QGridLayout* main_layout;
    QHBoxLayout* title_layout;
    QHBoxLayout* buttons_layout;
    QHBoxLayout* menu_layout;
    QScrollArea *scroll;
    QFrame* bottom_widget;
    QWidget* view_widget;
    QLabel *logo_icon;
    QFrame* title_widget;
    QLabel* title_icon;
    QLabel* title_text;
    QMap<QString, QAbstractButton*> buttons;
    QMap<QString, UserActionType> action_types;
    QMap<QString, QAction*> menus;
    QMenu *menu;
    QPushButton *menu_btn;

    void setActionIcon(const QString &key, const QIcon &icon);
    void addAction(const QString &key, UserActionType);
    void addActionButton(const QString &key, UserActionType);
    void addActionMenu(const QString &key, UserActionType);
    int newButtonPosition(UserActionType);
    int findButtonPosition(UserActionType);
    Qt::Alignment newButtonAlignment(UserActionType);
    QPixmap defaultActionIcon(UserActionType);
    void moveMenuButtonsToMenu();
};

class alWizardFace: public alWidgetPre<AWizardFace>
{
public:
    alWizardFace(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation);
    ~alWizardFace();
    void setAttr(const QString& name,const QString& value);
    void registerEvent(const QString&);
    QString postData() const ;
    QWidget* getViewWidget();
    QLayout* getViewLayout();
};

#endif
