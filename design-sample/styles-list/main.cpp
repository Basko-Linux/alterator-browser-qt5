
#include <QStyleFactory>
#include <QApplication>

int main(int argc,char **argv)
{
    QApplication app(argc, argv);

    qDebug( "Styles list: %s", qPrintable(QStyleFactory::keys().join(", ")) );
}
