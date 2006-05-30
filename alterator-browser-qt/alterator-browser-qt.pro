
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
QMAKE_CXXFLAGS += $RPM_OPT_FLAGS

SOURCES = browser.cc
SOURCES += layout.cc connection.cc widgets.cc sax.cc hacks.cc
SOURCES += main_window.cc updater.cc al_wizard_face.cc a_pixmaps.cc
HEADERS = global.hh
HEADERS += layout.hh widgets.hh connection.hh browser.hh sax.hh hacks.hh
HEADERS += main_window.hh updater.hh al_wizard_face.hh a_pixmaps.hh
DATA = *.pro *.spec

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
