#ifndef QTBROWSER_WIDGETS_HH
#define QTBROWSER_WIDGETS_HH

#include "al_widget.hh"

QLayout* findViewLayout(const QString& id);
QWidget* findQWidget(const QString& id);
alWidget* findAlWidgetById(const QString& id);
alWidget* findAlWidgetByName(const QString& name);
QList<alWidget*> findAlChildren(const QString& id);

//widgets
#include "al_main_widget.hh"
#include "al_label.hh"
#include "al_button.hh"
#include "al_radio.hh"
#include "al_edit.hh"
#include "al_textbox.hh"
#include "al_groupbox.hh"
#include "al_checkbox.hh"
#include "al_listbox.hh"
#include "al_combobox.hh"
#include "al_tabbox.hh"
#include "al_dialog.hh"
#include "al_box.hh"
#include "al_proxy.hh"
#include "al_progressbar.hh"
#include "al_tree.hh"
#include "al_helpplace.hh"
#include "al_slider.hh"
#include "al_separator.hh"
#include "al_spacer.hh"
#include "al_wizard_face.hh"
#include "al_gridbox.hh"
#include "al_spinbox.hh"

#endif
