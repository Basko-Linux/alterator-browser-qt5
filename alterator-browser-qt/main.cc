
#include <QApplication>
#include "main_window.hh"
#include "enums.hh"

MainWindow *main_window = 0;
Enums *enums = 0;

int main(int argc,char **argv)
{
    Q_INIT_RESOURCE(images);
    QApplication app(argc, argv);

    Enums enums_;
    enums = &enums_;

    MainWindow mw;
    mw.show();
    main_window = &mw;

    int ret = app.exec();
    main_window = 0;
    enums = 0;

    return ret;
}
