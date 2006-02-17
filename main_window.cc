
#include <QTimer>
#include <QGridLayout>

#include "browser.hh"
#include "connection.hh"
#include "main_window.hh"

extern Updater *updater;

MainWindow::MainWindow():
    MainWindow_t()
{
    new QGridLayout(this);
    QTimer::singleShot(0, this, SLOT(start()));
}

MainWindow::~MainWindow()
{
    if( updater )
    {
	delete updater;
	updater = 0;
    }
}

void MainWindow::start()
{
	updater = new Updater();
#if 1
	initConnection(getDocParser);
#endif

}
