#ifndef QTBROWSER_WIDGETS_HH
#define QTBROWSER_WIDGETS_HH

#include "al_widget.hh"

QLayout* findViewLayout(const QString& id);
QWidget* findQWidget(const QString& id);
alWidget* findAlWidgetById(const QString& id);
alWidget* findAlWidgetByName(const QString& name);
QList<alWidget*> findAlChildren(const QString& id);
void collectTabIndex(QList<QString>&, QMap<QString, QMap<int,QWidget*> >&, alWidget* wdg, int);

//widgets
#include "widgets/al_main_widget.hh"
#include "widgets/al_label.hh"
#include "widgets/al_button.hh"
#include "widgets/al_radio.hh"
#include "widgets/al_edit.hh"
#include "widgets/al_textbox.hh"
#include "widgets/al_groupbox.hh"
#include "widgets/al_checkbox.hh"
#include "widgets/al_listbox.hh"
#include "widgets/al_combobox.hh"
#include "widgets/al_tabbox.hh"
#include "widgets/al_dialog.hh"
#include "widgets/al_box.hh"
#include "widgets/al_proxy.hh"
#include "widgets/al_progressbar.hh"
#include "widgets/al_tree.hh"
#include "widgets/al_helpplace.hh"
#include "widgets/al_slider.hh"
#include "widgets/al_separator.hh"
#include "widgets/al_spacer.hh"
#include "widgets/al_wizard_face.hh"
#include "widgets/al_gridbox.hh"
#include "widgets/al_spinbox.hh"
#include "widgets/al_dateedit.hh"
#include "widgets/al_timeedit.hh"
#include "widgets/al_slideshow.hh"
#include "widgets/al_splitbox.hh"

#endif
