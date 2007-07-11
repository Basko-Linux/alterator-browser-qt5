
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
    str2action["language"] = AlteratorRequestLanguage;
    str2action["retry"] = AlteratorRequestRetry;
    str2action["constraints-add"]   = AlteratorRequestCnstrAdd;
    str2action["constraints-clear"] = AlteratorRequestCnstrClear;
    str2action["constraints-apply"] = AlteratorRequestCnstrApply;

    str2widget["root"] = WMainWidget;
    str2widget["box"] = WBox;
    str2widget["vbox"] = WVBox;
    str2widget["hbox"] = WHBox;
    str2widget["button"] = WButton;
    str2widget["radio"] = WRadio;
    str2widget["label"] = WLabel;
    str2widget["edit"] = WEdit;
    str2widget["textbox"] = WTextBox;
    str2widget["help-place"] = WHelpPlace;
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
    str2widget["slideshow"] = WSlideShow;
    str2widget["splitbox"] = WSplitBox;
    str2widget["wizardface"] = WWizardFace;

    str2u_action["abort"]    = UserActionAbort;
    str2u_action["finish"]   = UserActionFinish;
    str2u_action["help"]     = UserActionHelp;
    str2u_action["apply"]    = UserActionApply;
    str2u_action["cancel"]   = UserActionCancel;
    str2u_action["backward"] = UserActionBackward;
    str2u_action["forward"]  = UserActionForward;
    str2u_action["generic"]  = UserActionGeneric;
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

UserActionType Enums::strToUserAction(const QString &str)
{
    return str2u_action[str];
}