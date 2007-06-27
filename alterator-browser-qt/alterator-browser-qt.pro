
NAME=alterator-browser-qt
VERSION=$$system(cat VERSION)
BRANCH=head

TOPDIR=$$system("pwd")
RPM_TOPDIR=$$TOPDIR/RPM
RPM_TMPDIR=$$RPM_TOPDIR/RPM/TMP
RPM_SOURCEDIR=$$RPM_TOPDIR/SOURCES
RPM_SPECSDIR=$$TOPDIR
RPM_SRPMSDIR=$$RPM_TOPDIR/SRPMS

TEMPLATE = app
QT += xml
QMAKE_CXXFLAGS += -g -O0 -pedantic $(RPM_OPT_FLAGS)

SOURCES =  main.cc
SOURCES += mailbox.cc connection.cc widgets.cc sax.cc messagebox.cc
SOURCES += main_window.cc a_pixmaps.cc utils.cc constraints.cc help_browser.cc
SOURCES += al_wizard_face.cc al_dialog.cc al_widget.cc al_main_widget.cc
SOURCES += al_edit.cc al_textbox.cc al_spacer.cc al_box.cc al_groupbox.cc
SOURCES += al_slider.cc al_tree.cc al_listbox.cc al_tabbox.cc al_gridbox.cc
SOURCES += al_spinbox.cc al_separator.cc al_progressbar.cc al_label.cc
SOURCES += al_button.cc al_radio.cc al_checkbox.cc al_combobox.cc al_proxy.cc
SOURCES += al_helpplace.cc al_dateedit.cc al_timeedit.cc al_slideshow.cc
SOURCES += al_splitbox.cc

HEADERS =  global.hh
HEADERS += mailbox.hh connection.hh widgets.hh sax.hh messagebox.hh
HEADERS += main_window.hh a_pixmaps.hh utils.hh constraints.hh help_browser.hh
HEADERS += al_wizard_face.hh al_dialog.hh al_widget.hh al_main_widget.hh
HEADERS += al_edit.hh al_textbox.hh al_spacer.hh al_box.hh al_groupbox.hh
HEADERS += al_slider.hh al_tree.hh al_listbox.hh al_tabbox.hh al_gridbox.hh
HEADERS += al_spinbox.hh al_separator.hh al_progressbar.hh al_label.hh
HEADERS += al_button.hh al_radio.hh al_checkbox.hh al_combobox.hh al_proxy.hh
HEADERS += al_helpplace.hh al_dateedit.hh al_timeedit.hh al_slideshow.hh
SOURCES += al_splitbox.hh

DATA = *.pro *.spec
RESOURCES = images.qrc

FORMS += help_browser.ui

target.path = /usr/bin/
INSTALLS += target

# custom target to create tarball from CVS
tball.target = tball
tball.input = NAME VERSION SOURCES HEADERS DATA RPM_SOURCEDIR RPM_TMPDIR
tball.commands = \
	mkdir -p $$RPM_SOURCEDIR ; \
	mkdir -p $$RPM_TMPDIR/$$NAME-$$VERSION ; \
	cp -ar $$SOURCES $$HEADERS $$DATA $$RPM_TMPDIR/$$NAME-$$VERSION ; \
	pushd $$RPM_TMPDIR ; \
	tar -cjpf $$RPM_SOURCEDIR/$$NAME-$$sprintf("%1.tar.bz2",$$VERSION) $$NAME-$$VERSION ; \
	popd ; \
	rm -rf $$RPM_TMPDIR/$$NAME-$$VERSION

# custom target to build .src.rpm
srpm.target = srpm
srpm.input = NAME TOPDIR RPM_TOPDIR RPM_SOURCEDIR RPM_TMPDIR RPM_SRPMSDIR RPM_SPECSDIR
srpm.depends = tball
srpm.commands = \
	rpmbuild -bs \
	    --define "_topdir $$RPM_TOPDIR" \
	    --define "_sourcedir $$RPM_SOURCEDIR" \
	    --define "_tmpdir $$RPM_TMPDIR" \
	    --define "_specdir $$RPM_SPECSDIR" \
	    --define "_srpmsdir $$RPM_SRPMSDIR" \
	    $$sprintf("%1.spec",$$NAME)

# custom target to build .rpm
rpm.target = rpm
rpm.input = NAME TOPDIR RPM_TOPDIR RPM_SOURCEDIR RPM_TMPDIR RPM_SRPMSDIR RPM_SPECSDIR
rpm.depends = tball
rpm.commands = \
	rpmbuild -bb \
	    --define "_topdir $$RPM_TOPDIR" \
	    --define "_sourcedir $$RPM_SOURCEDIR" \
	    --define "_tmpdir $$RPM_TMPDIR" \
	    --define "_specdir $$RPM_SPECSDIR" \
	    --define "_srpmsdir $$RPM_SRPMSDIR" \
	    $$sprintf("%1.spec",$$NAME)

# custom target to clean rpm files
clear.target = clear
clear.input = RPM_TOPDIR
clear.commands = \
	rm -rf $$RPM_TOPDIR

# somewhere else in the *.pro file
QMAKE_EXTRA_UNIX_TARGETS += tball srpm rpm clear
