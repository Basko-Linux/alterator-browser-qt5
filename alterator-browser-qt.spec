Name: alterator-browser-qt
Version: 2.6
Release: alt1

Source:%name-%version.tar.bz2

Summary: X11 Qt interface driver for alterator
License: GPL
Group: System/Configuration/Other

Provides: alterator-browser
Requires: alterator-common >= 2.6

Provides: alterator-look-qt
Obsoletes: alterator-look-qt


# Automatically added by buildreq on Thu Dec 08 2005
BuildRequires: fontconfig freetype2 gcc-c++ libqt4-core libqt4-devel libstdc++-devel qt4-settings

BuildRequires: gcc-c++ guile16-devel libdbus-devel libdbus-qt-devel libqt3-devel libstdc++-devel pkgconfig

%description
X11 Qt interface driver for alterator

%prep
%setup -q

%build
%_libdir/qt4/bin/qmake
%make_build


%install
%make INSTALL_ROOT=$RPM_BUILD_ROOT install

%files
%_bindir/*

%changelog
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

