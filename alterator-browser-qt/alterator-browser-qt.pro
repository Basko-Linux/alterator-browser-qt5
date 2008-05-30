
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
QT += xml network
QMAKE_CXXFLAGS += -pedantic $(RPM_OPT_FLAGS)
PRECOMPILED_DIR = .pch/
OBJECTS_DIR = .obj
MOC_DIR = .moc
RCC_DIR = .rcc
UI_DIR = .uic

SOURCES =  main.cc
SOURCES += flowlayout.cpp
SOURCES += mailbox.cc connection.cc sax.cc messagebox.cc
SOURCES += main_window.cc a_pixmaps.cc utils.cc constraints.cc help_browser.cc
SOURCES += enums.cc splashscreen.cc widgetlist.cc
SOURCES += al_widget.cc widgets/al_*.cc

HEADERS =  global.hh
HEADERS += flowlayout.hh
HEADERS += mailbox.hh connection.hh widgets.hh sax.hh messagebox.hh
HEADERS += main_window.hh a_pixmaps.hh utils.hh constraints.hh help_browser.hh
HEADERS += enums.hh splashscreen.hh widgetlist.hh
HEADERS += al_widget.hh widgets/al_*.hh

DATA = *.pro *.spec
RESOURCES = images.qrc

FORMS += help_browser.ui

TRANSLATIONS = \
    translations/alterator_browser_qt_ru.ts \
    translations/alterator_browser_qt_uk.ts \
    translations/alterator_browser_qt_be.ts \
    translations/alterator_browser_qt_tt.ts \
    translations/alterator_browser_qt_kk.ts

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
