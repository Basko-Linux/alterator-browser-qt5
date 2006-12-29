#ifndef QTBROWSER_AL_WIZARDFACE_HH
#define QTBROWSER_AL_WIZARDFACE_HH

#include <QPushButton>
#include <QLabel>
#include <QSignalMapper>
#include <QMenu>

#include "widgets.hh"

class AWizardFaceStepList: public QList< QPair<QString, QString> >
{
public:
    AWizardFaceStepList() {};
    ~AWizardFaceStepList() {};
};

class AWizardFace: public QDialog
{
    Q_OBJECT
public:
    AWizardFace( QWidget *parent=0, Qt::WFlags f=0 );
    ~AWizardFace();

    enum ActionType
    {
	ActionGeneric,
	ActionAbort,
	ActionFinish,
	ActionHelp,
	ActionApply,
	ActionCancel,
	ActionBackward,
	ActionForward,
    };

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
    void onEnter();

signals:
    void stepSelected();
    void actionSelected();
// private
    void actionSelected(const QString&);

private slots:
    void onSelectAction(const QString&);
    void onSelectStep(QListWidgetItem*);

protected:
#if 0
    void keyPressEvent(QKeyEvent*);
#endif

private:
    AWizardFaceStepList steplist;
    int current_step;
    QString current_action;
    QSignalMapper *action_signal_mapper;
    QGridLayout* main_layout;
    QHBoxLayout* title_layout;
    QHBoxLayout* buttons_layout;
    QFrame* buttons_widget;
    QWidget* view_widget;
    QFrame* title_widget;
    QLabel* title_icon;
    QLabel* title_text;
    QMap<QString,AWizardFace::ActionType> key2type;
    QMap<QString, QAbstractButton*> buttons;
    QMap<QString, ActionType> button_types;
    QMap<QString, QAction*> menus;
    QMenu *menu;
    QPushButton *menu_btn;

    void addAction(const QString &key, ActionType);
    int newButtonPosition(ActionType);
    int findButtonPosition(ActionType);
    Qt::Alignment newButtonAlignment(ActionType);
    QPixmap defaultActionIcon(ActionType);
    QString translateActionText(const QString&);
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
