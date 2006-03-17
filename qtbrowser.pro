TEMPLATE = app

VERSION=-$(shell cat VERSION)

QT += xml

SOURCES += browser.cc layout.cc connection.cc widgets.cc sax.cc hacks.cc
SOURCES += main_window.cc updater.cc
HEADERS += layout.hh widgets.hh connection.hh browser.hh sax.hh hacks.hh
HEADERS += main_window.hh updater.hh

target.path = /usr/bin/
INSTALLS += target
