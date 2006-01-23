TEMPLATE = app

VERSION=-2.6

QT += xml

SOURCES += browser.cc layout.cc connection.cc widgets.cc sax.cc
HEADERS += layout.hh widgets.hh connection.hh browser.hh sax.hh

target.path = /usr/bin/
INSTALLS += target
