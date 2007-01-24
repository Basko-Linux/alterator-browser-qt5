Name: alterator-browser-qt
Version: 2.9.32
Release: alt1

Source:%name-%version.tar

Summary: X11 Qt interface driver for alterator
License: GPL
Group: System/Configuration/Other

Requires: alterator-common >= 2.9-alt0.14
Requires: alterator-icons
PreReq(post,preun): alternatives >= 0.2

Provides: alterator-browser
Provides: alterator-browser-x11
Requires: libqt4-core >= %{get_version libqt4-core}

Obsoletes: alterator-look-qt

BuildRequires: libalternatives-devel
BuildRequires: fontconfig freetype2 gcc-c++ libqt4-devel libstdc++-devel

%description
X11 Qt interface driver for alterator


%prep
%setup -q
%_qt4dir/bin/qmake -spec default
#sed -i "s|^\s*CFLAGS\s*=.*$|CFLAGS = %optflags -D_REENTRANT \$(DEFINES)|" Makefile
#sed -i "s|^\s*CXXFLAGS\s*=.*$|CXXFLAGS = %optflags -D_REENTRANT \$(DEFINES)|" Makefile


%build
%make_build


%install
%make INSTALL_ROOT=%buildroot install
ln -s %name %buildroot/%_bindir/qtbrowser

mkdir -p %buildroot/%_altdir
cat >%buildroot/%_altdir/%name <<__EOF__
%_bindir/alterator-browser-x11	%_bindir/%name 99
__EOF__


%post
%post_register_alternatives %name -- %name
%update_alternatives
%preun
%preun_unregister_alternatives %name


%files
%config %_altdir/%name
%_bindir/*

%changelog
* Wed Jan 24 2007 Sergey V Turchin <zerg at altlinux dot org> 2.9.32-alt1
- add gridbox widget

* Mon Jan 15 2007 Sergey V Turchin <zerg at altlinux dot org> 2.9.31-alt1
- fix splitter, rename to separator
- fix help button position
- add orientation to dialog mainwidget and wizardface

* Fri Jan 12 2007 Sergey V Turchin <zerg at altlinux dot org> 2.9.30-alt1
- temporary add width/height for dialogs

* Wed Jan 10 2007 Sergey V Turchin <zerg at altlinux dot org> 2.9.29-alt1
- move browser socket into alterator sub-directory

* Wed Jan 10 2007 Sergey V Turchin <zerg at altlinux dot org> 2.9.28-alt1
- add orientation(default vertical) attribute to container widgets

* Tue Jan 09 2007 Sergey V Turchin <zerg at altlinux dot org> 2.9.27-alt2
- fix cursor names

* Fri Dec 29 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.27-alt1
- add cursor attribute
- add actions to dialog
- add spinbox widget

* Tue Dec 26 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.26-alt1
- add wizardface default tranlations
- add help item to wizardface by default

* Fri Dec 22 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.25-alt1
- add internal messaging system support

* Tue Dec 19 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.24-alt1
- add widgets: vtabbox, htabbox, vtab-page, htab-page
- show help browser when wizardface menu item activated

* Thu Dec 14 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.23-alt4
- anonymize widget and all it's children before delete

* Wed Dec 13 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.23-alt3
- fix updated signal from checkbox

* Wed Dec 13 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.23-alt2
- set default help text

* Wed Dec 13 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.23-alt1
- add constraints to edit, textedit
- add messagebox buttons translation
- display step numers from 1 in wizardface

* Tue Dec 12 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.22-alt1
- change steps visibility

* Tue Dec 12 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.21-alt3
- turn off children-align attribute

* Mon Dec 11 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.21-alt2
- don't show empty steps list

* Mon Dec 11 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.21-alt1
- return children-align attribute
- add align attribute

* Fri Dec 08 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.20-alt1
- implement help browser
- reduce events flood from slider

* Thu Dec 07 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.19-alt2
- fix messagebox buttons size

* Thu Dec 07 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.19-alt1
- add max-width, max-height attributes

* Mon Dec 04 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.18-alt1
- fix full-screen mode

* Mon Dec 04 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.17-alt3
- fix LANGUAGE detection

* Fri Dec 01 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.17-alt2
- fix messagebox borders and placement
- add workaround against showing dialogs when loaded

* Thu Nov 30 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.17-alt1
- fix button and label size

* Thu Nov 30 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.16-alt1
- fix cleaning widgets

* Wed Nov 29 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.15-alt1
- add base constraints support

* Wed Nov 29 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.14-alt1
- add spacer widget

* Tue Nov 21 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.13-alt3
- fix tree/columns

* Tue Nov 21 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.13-alt2
- fix listbox/double-click, wizardface/title

* Mon Nov 20 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.13-alt1
- add rows-clear attribute to listbox/combobox

* Fri Nov 17 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.12-alt4
- fix wizardface menu actions attributes

* Fri Nov 17 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.12-alt3
- stable wizardface API

* Fri Nov 17 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.12-alt2
- fix current attribute for wizardface

* Thu Nov 16 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.12-alt1
- new wizardface API

* Mon Nov 13 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.11-alt1
- fix document:popup-*
- new wizardface interface

* Fri Nov 10 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.10-alt1
- update to new alterator

* Thu Nov 02 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.9-alt2
- small widgets layout fix

* Tue Oct 31 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.9-alt1
- ignore attributes: layout-policy, width, height, align, children-align,
  background-color

* Mon Oct 23 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.8-alt1
- fix window manager detection

* Fri Oct 13 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.7-alt1
- fix dialog borders

* Thu Oct 12 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.6-alt1
- detect Window Manager

* Tue Oct 10 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.5-alt1
- update to new alterator

* Mon Oct 09 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.4-alt4
- update to new alterator

* Mon Oct 09 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.4-alt3
- add some fixes for new alterator

* Fri Oct 06 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.4-alt2
- fix draw primitives with Qt-4.2

* Tue Sep 26 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.4-alt1
- apply default icons to wizardface buttons
- arrange buttons in wizardface

* Mon Sep 25 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.3-alt1
- autowrap text on labels
- add current and title attributes to wizardface
- require alterator-icons

* Wed Jul 12 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.2-alt1
- update to new alterator

* Tue Jun 20 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.1-alt5
- add changed event to silder

* Mon Jun 19 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.1-alt4
- add value attribute to slider

* Mon Jun 19 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.1-alt3
- fix slider attributes

* Thu Jun 15 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.1-alt2
- add slider,splitter widgets

* Wed Jun 07 2006 Sergey V Turchin <zerg at altlinux dot org> 2.9.1-alt1
- update to new alterator

* Tue Jun 06 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt24
- turn off modeless dialogs

* Tue May 30 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt23
- add fix for tree

* Tue May 30 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt22
- make listbox clicked by space 

* Wed May 24 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt21
- fix compile with new gcc

* Thu May 04 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt20
- update to new alterator

* Fri Apr 28 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt19
- update to vector based model
- fix problems with multiline textboxes

* Fri Apr 21 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt18
- add scrolling to wizardface inner widget

* Thu Apr 20 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt17
- improve pixmap theme handling

* Fri Apr 14 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt16
- change language selection order

* Thu Apr 13 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt15
- rename some standard pixmaps to better understanding by ID

* Wed Apr 12 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt14
- update standatd pixmaps list

* Tue Apr 11 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt13
- rename items to rows
- add pixmaps caching
- add support for internal Qt pixmaps

* Thu Apr 06 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt12
- add alTree fixes

* Wed Apr 05 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt11
- add patch fix for item-text and item-pixmap in multicolon listbox

* Wed Apr 05 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt10
- don't use alMultiListBox for 1 column

* Tue Apr 04 2006 Sergey V Turchin <zerg at altlinux dot org> 2.6.1-alt9
- add alMultiListBox
- add alHelpPlace

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

