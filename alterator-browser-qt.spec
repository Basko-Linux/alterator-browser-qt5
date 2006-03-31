Name: alterator-browser-qt
Version: 2.6.1
Release: alt8

Source:%name-%version.tar.bz2

Summary: X11 Qt interface driver for alterator
License: GPL
Group: System/Configuration/Other

Provides: alterator-browser
Requires: alterator-common >= 2.6

Provides: alterator-look-qt
Obsoletes: alterator-look-qt


BuildRequires: fontconfig freetype2 gcc-c++ libqt4-devel libstdc++-devel
BuildRequires: gcc-c++ guile16-devel libdbus-devel libdbus-qt-devel libqt3-devel libstdc++-devel pkg-config

%description
X11 Qt interface driver for alterator

%prep
%setup -q
%_qt4dir/bin/qmake -spec default
sed -i "s|^\s*CFLAGS\s*=.*$|CFLAGS = %optflags -D_REENTRANT \$(DEFINES)|" Makefile
sed -i "s|^\s*CXXFLAGS\s*=.*$|CXXFLAGS = %optflags -D_REENTRANT \$(DEFINES)|" Makefile

%build
%make_build


%install
%make INSTALL_ROOT=$RPM_BUILD_ROOT install

%files
%_bindir/*

%changelog
* Fri Mar 31 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt8
- add alTree

* Fri Mar 31 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt7
- fix combobox post items

* Thu Mar 30 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt6
- add (combobox alterability)

* Tue Mar 28 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt5
- save window geometry on exit

* Thu Mar 23 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt4
- add (listbox on-double-click)

* Tue Mar 21 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt3
- add progressbar
- add alProxy

* Fri Mar 17 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt2
- add on-click, on-return to listbox
- add on-return to edit
- add on-click to listbox

* Wed Mar 15 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt1
- add align for label
- add widget named root and symlink to vbox
- add sax optimization

* Tue Mar 14 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6-alt14
- clear listbox and combobox when add list of items

* Thu Mar 09 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6-alt13
- add items property to listbox and combobox

* Mon Mar 06 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6-alt12
- fix current item when listbox on-select

* Wed Feb 22 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6-alt11
- separate main window
- start full-screen without window manager

* Tue Feb 21 2006 Stanislav Ievlev <inger@altlinux.org> 2.6-alt10
- improve language selection

* Wed Feb 08 2006 Stanislav Ievlev <inger@altlinux.org> 2.6-alt9
- listbox fixes (indentation + auto-scrolling)

* Tue Feb 07 2006 Stanislav Ievlev <inger@altlinux.org> 2.6-alt8.1
- little layout fixes

* Mon Feb 06 2006 Stanislav Ievlev <inger@altlinux.org> 2.6-alt8
- resurrect old hacks for QDialog
- added support for full-screen

* Fri Jan 27 2006 Stanislav Ievlev <inger@altlinux.org> 2.6-alt7.2
- added support for splashes

* Fri Jan 27 2006 Stanislav Ievlev <inger@altlinux.org> 2.6-alt7.1
- added support for tooltips

* Mon Jan 23 2006 Stanislav Ievlev <inger@altlinux.org> 2.6-alt7
- change DOM parser to SAX

* Fri Jan 20 2006 Stanislav Ievlev <inger@altlinux.org> 2.6-alt6.4
- tabbox current property fix

* Thu Jan 19 2006 Stanislav Ievlev <inger@altlinux.org> 2.6-alt6.3
- performance fix

* Wed Jan 18 2006 Stanislav Ievlev <inger@altlinux.org> 2.6-alt6.2
- layout bugfix

* Mon Jan 16 2006 Stanislav Ievlev <inger@altlinux.org> 2.6-alt6.1
- replace QListWidget with QTreeWidget

* Wed Jan 11 2006 Stanislav Ievlev <inger@altlinux.org> 2.6-alt6
- added support for radio-buttons and spacers

* Thu Dec 22 2005 Stanislav Ievlev <inger@altlinux.org> 2.6-alt5
- added support for timers

* Tue Dec 20 2005 Stanislav Ievlev <inger@altlinux.org> 2.6-alt4
- improve initial connection
- ressurect locale support

* Mon Dec 19 2005 Stanislav Ievlev <inger@altlinux.org> 2.6-alt3
- fixed layout

* Fri Dec 16 2005 Stanislav Ievlev <inger@altlinux.org> 2.6-alt2
- fixed xml support

* Thu Dec 15 2005 Stanislav Ievlev <inger@altlinux.org> 2.6-alt1.1
- fixed provides

* Thu Dec 08 2005 Stanislav Ievlev <inger@altlinux.org> 2.6-alt1
- now as qtbrowser

* Fri Dec 02 2005 Stanislav Ievlev <inger@altlinux.org> 2.5-alt0.4.2
- little code cleanup

* Thu Dec 01 2005 Stanislav Ievlev <inger@altlinux.org> 2.5-alt0.4.1
- fixed support for pixmap attribute of button

* Wed Nov 30 2005 Stanislav Ievlev <inger@altlinux.org> 2.5-alt0.4
- added children-align property support

* Tue Nov 29 2005 Stanislav Ievlev <inger@altlinux.org> 2.5-alt0.3
- fixed bugs

* Thu Nov 03 2005 Stanislav Ievlev <inger@altlinux.org> 2.5-alt0.2
- improvements

* Wed Nov 02 2005 Stanislav Ievlev <inger@altlinux.org> 2.5-alt0.1
- build current unstable version

* Wed Aug 31 2005 Sergey V Turchin <zerg at altlinux dot org> 2.1-alt0.3
- add border to splash

* Tue Aug 30 2005 Sergey V Turchin <zerg at altlinux dot org> 2.1-alt0.2
- new widgets layout
- scrollview widget for module window
- fix splash background color

* Tue Aug 30 2005 Stanislav Ievlev <inger@altlinux.org> 2.1-alt0.1
- initial build (specially for zerg)

