#ifndef QT_BROWSER_ENUMS_HH
#define QT_BROWSER_ENUMS_HH

#include <QString>
#include <QMap>

enum AlteratorRequestActionType
{
    AlteratorRequestUnknown = 0,
    AlteratorRequestNew = 1,
    AlteratorRequestClose = 2,
    AlteratorRequestClean = 3,
    AlteratorRequestSet = 4,
    AlteratorRequestEvent = 5,
    AlteratorRequestSplash = 6,
    AlteratorRequestStart = 7,
    AlteratorRequestStop = 8,
    AlteratorRequestMessage = 9,
    AlteratorRequestLanguage = 10,
    AlteratorRequestCnstrAdd = 11,
    AlteratorRequestCnstrClear = 12,
    AlteratorRequestCnstrApply = 13,
    AlteratorRequestRetry = 14
};

enum AlteratorWidgetType {
    WUnknown = 0,       WWizardFace = 1,
    WMainWidget = 2,    WDialog = 3,
    WLabel = 4,         WButton = 5,
    WRadio = 6,         WEdit = 7,
    WTextBox = 8,       WGroupBox = 9,
    WHGroupBox = 10,    WVGroupBox = 11,
    WCheckBox = 12,     WListBox = 13,
    WMultiListBox = 14, WComboBox = 15,
    WTabBox = 16,       WVTabBox = 17,
    WHTabBox = 18,      WTabPage = 19,
    WHTabPage = 20,     WVTabPage = 21,
    WBox = 22,          WVBox = 23,
    WHBox = 24,         WGridBox = 25,
    WSplitBox = 26,     WProgressBar = 27,
    WTree = 28,         WSlider = 29,
    WSeparator = 30,    WSpinBox = 31,
    WHelpPlace = 32,    WProxy = 33,
    WSpacer = 34,       WDateEdit = 35,
    WTimeEdit = 36,     WSlideShow = 37
};

enum UserActionType
{
    UserActionGeneric = 0,
    UserActionAbort = 1,
    UserActionFinish = 2,
    UserActionHelp = 3,
    UserActionApply = 4,
    UserActionCancel = 5,
    UserActionBackward = 6,
    UserActionForward = 7
};

class Enums
{
public:
    Enums();
    ~Enums();

    AlteratorRequestActionType strToRequestAction(const QString&);
    AlteratorWidgetType strToWidget(const QString&);
    UserActionType strToUserAction(const QString&);

private:
    QMap<QString,AlteratorRequestActionType> str2action;
    QMap<QString,AlteratorWidgetType> str2widget;
    QMap<QString,UserActionType> str2u_action;
};

#endif
