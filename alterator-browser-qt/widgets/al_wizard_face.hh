#ifndef QTBROWSER_AL_WIZARDFACE_HH
#define QTBROWSER_AL_WIZARDFACE_HH

#include <QPushButton>
#include <QLabel>
#include <QSignalMapper>
#include <QMenu>

#include "enums.hh"
#include "widgets.hh"

class AWizardFaceStepList: public QList< QPair<QString, QString> >
{
public:
    AWizardFaceStepList() {};
    ~AWizardFaceStepList() {};
};

class AWizardFace: public QWidget
{
    Q_OBJECT
public:
    AWizardFace( QWidget *parent=0, Qt::WFlags f=0 );
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

private:
    AWizardFaceStepList steplist;
    int current_step;
    QString current_action;
    QSignalMapper *action_signal_mapper;
    QGridLayout* main_layout;
    QHBoxLayout* title_layout;
    QHBoxLayout* buttons_layout;
    QHBoxLayout* menu_layout;
    QFrame* bottom_widget;
    QWidget* view_widget;
    QLabel *logo_icon;
    QFrame* title_widget;
    QLabel* title_icon;
    QLabel* title_text;
    QMap<QString, QAbstractButton*> buttons;
    QMap<QString, UserActionType> button_types;
    QMap<QString, QAction*> menus;
    QMenu *menu;
    QPushButton *menu_btn;

    void addAction(const QString &key, UserActionType);
    int newButtonPosition(UserActionType);
    int findButtonPosition(UserActionType);
    Qt::Alignment newButtonAlignment(UserActionType);
    QPixmap defaultActionIcon(UserActionType);
    QString translateActionText(const QString&);
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
