
#include <QApplication>
#include "browser.hh"
#include "enums.hh"

Browser *browser = 0;
Enums *enums = 0;

int main(int argc,char **argv)
{
    Q_INIT_RESOURCE(images);
    QApplication app(argc, argv);

    Enums enums_;
    enums = &enums_;

    Browser mw;
    mw.show();
    browser = &mw;

    int ret = app.exec();
    browser = 0;
    enums = 0;

    return ret;
}
