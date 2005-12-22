TEMPLATE = app

VERSION=-2.6

QT += xml

SOURCES += browser.cc layout.cc connection.cc widgets.cc
HEADERS += layout.hh widgets.hh connection.hh browser.hh

target.path = /usr/bin/
INSTALLS += target