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
    AlteratorRequestFile = 10,
    AlteratorRequestLanguage = 11,
    AlteratorRequestCnstrAdd = 12,
    AlteratorRequestCnstrClear = 13,
    AlteratorRequestCnstrApply = 14,
    AlteratorRequestRetry = 15
};

enum AlteratorWidgetType {
    WUnknown = 0,       WWizardFace = 10,
    WCenterFace = 15,
    WMainWidget = 20,    WDialog = 30,
    WLabel = 40,
    WButton = 50,        WLinkButton = 51,
    WRadio = 60,         WEdit = 70,
    WTextBox = 80,       WGroupBox = 90,
    WHGroupBox = 100,    WVGroupBox = 110,
    WCheckBox = 120,     WComboBox = 130,
    WListBox = 140,      WRadioListBox = 150,
    WMultiListBox = 160, WCheckListBox = 170,
    WTabBox = 190,       WVTabBox = 200,
    WHTabBox = 210,      WTabPage = 220,
    WHTabPage = 230,     WVTabPage = 240,
    WSplitBox = 260,     WProgressBar = 270,
    WTree = 280,         WSlider = 290,
    WSeparator = 300,    WSpinBox = 310,
    WProxy = 330,
    WSpacer = 340,       WDateEdit = 350,
    WTimeEdit = 360,     WSlideShow = 370,
    WBox = 380,          WVBox = 390,
    WHBox = 400,         WGridBox = 410
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

typedef int BrowserEventTypes;
enum BrowserEventType
{
    BrowserEventUnknown = 0,
    BrowserEventClicked = 1,
    BrowserEventRreturnPressed = 2,
    BrowserEventSelected = 3,
    BrowserEventDoubleClicked = 4,
    BrowserEventToggled = 5,
    BrowserEventUpdate = 6
};

class Enums
{
public:
    Enums();
    ~Enums();

    AlteratorRequestActionType strToRequestAction(const QString&);
    AlteratorWidgetType strToWidget(const QString&);
    QString widgetToStr(const AlteratorWidgetType);
    UserActionType strToUserAction(const QString&);
    BrowserEventType strToBrowserEvent(const QString&);
    QString browserEventToStr(const BrowserEventType);

private:
    QMap<QString,AlteratorRequestActionType> str2action;
    QMap<QString,AlteratorWidgetType> str2widget;
    QMap<QString,UserActionType> str2u_action;
    QMap<QString,BrowserEventType> str2event;
};

extern Enums *enums;

#endif
