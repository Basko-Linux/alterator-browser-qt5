
#include <QApplication>
#include "main_window.hh"
MainWindow *main_window = 0;

int main(int argc,char **argv)
{
    Q_INIT_RESOURCE(images);
    QApplication app(argc, argv);

    MainWindow mw;
    mw.show();
    main_window = &mw;
    int ret = app.exec();
    main_window = 0;
    return ret;
}
