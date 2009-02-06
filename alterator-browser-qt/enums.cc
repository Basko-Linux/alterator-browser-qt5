
#include "enums.hh"

Enums::Enums()
{
    str2action["new"] = AlteratorRequestNew;
    str2action["close"] = AlteratorRequestClose;
    str2action["clean"] = AlteratorRequestClean;
    str2action["set"] = AlteratorRequestSet;
    str2action["create-event"] = AlteratorRequestEvent;
    str2action["splash"] = AlteratorRequestSplash;
    str2action["start"] = AlteratorRequestStart;
    str2action["stop"] = AlteratorRequestStop;
    str2action["messagebox"] = AlteratorRequestMessage;
    str2action["fileselect"] = AlteratorRequestFile;
    str2action["language"] = AlteratorRequestLanguage;
    str2action["retry"] = AlteratorRequestRetry;

    str2widget["root"] = WMainWidget;
    str2widget["box"] = WBox;
    str2widget["vbox"] = WVBox;
    str2widget["hbox"] = WHBox;
    str2widget["button"] = WButton;
    str2widget["linkbutton"] = WLinkButton;
    str2widget["radio"] = WRadio;
    str2widget["label"] = WLabel;
    str2widget["edit"] = WEdit;
    str2widget["textbox"] = WTextBox;
    str2widget["groupbox"] = WGroupBox;
    str2widget["gridbox"] = WGridBox;
    str2widget["checkbox"] = WCheckBox;
    str2widget["tree"] = WTree;
    str2widget["combobox"] = WComboBox;
    str2widget["tabbox"] = WTabBox;
    str2widget["tab-page"] = WTabPage;
    str2widget["progressbar"] = WProgressBar;
    str2widget["slider"] = WSlider;
    str2widget["separator"] = WSeparator;
    str2widget["spacer"] = WSpacer;
    str2widget["spinbox"] = WSpinBox;
    str2widget["dateedit"] = WDateEdit;
    str2widget["timeedit"] = WTimeEdit;
    str2widget["listbox"] = WListBox;
    str2widget["radiolistbox"] = WRadioListBox;
    str2widget["multilistbox"] = WMultiListBox;
    str2widget["checklistbox"] = WCheckListBox;
    str2widget["slideshow"] = WSlideShow;
    str2widget["splitbox"] = WSplitBox;
    str2widget["wizardface"] = WWizardFace;
    str2widget["centerface"] = WCenterFace;

    str2u_action["abort"]    = UserActionAbort;
    str2u_action["finish"]   = UserActionFinish;
    str2u_action["help"]     = UserActionHelp;
    str2u_action["apply"]    = UserActionApply;
    str2u_action["cancel"]   = UserActionCancel;
    str2u_action["backward"] = UserActionBackward;
    str2u_action["forward"]  = UserActionForward;
    str2u_action["generic"]  = UserActionGeneric;

    str2event["udpate"]        = BrowserEventUpdate;
    str2event["clicked"]        = BrowserEventClicked;
    str2event["return-pressed"] = BrowserEventRreturnPressed;
    str2event["selected"]       = BrowserEventSelected;
    str2event["double-clicked"] = BrowserEventDoubleClicked;
    str2event["toggled"]        = BrowserEventToggled;
    str2event["changed"]        = BrowserEventChanged;
}

Enums::~Enums()
{
}

AlteratorRequestActionType Enums::strToRequestAction(const QString &str)
{
    return str2action[str];
}

AlteratorWidgetType Enums::strToWidget(const QString &str)
{
    return str2widget[str];
}

QString Enums::widgetToStr(const AlteratorWidgetType t)
{
    QString ret = str2widget.key(t);
    if( ret.isEmpty() ) ret = "__undefined__";
    return ret;
}

UserActionType Enums::strToUserAction(const QString &str)
{
    return str2u_action[str];
}

BrowserEventType Enums::strToBrowserEvent(const QString &str)
{
    return str2event[str];
}

QString Enums::browserEventToStr(const BrowserEventType t)
{
    QString ret = str2event.key(t);
    if( ret.isEmpty() ) ret = "__undefined__";
    return ret;
}
