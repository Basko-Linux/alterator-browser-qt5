#include <iostream>

#include <QApplication>

#include "connection.hh"
#include "main_window.hh"
#include "widgets.hh"
#include "a_pixmaps.hh"
#include "messagebox.hh"
#include "constraints.hh"
#include "mailbox.hh"
#include "utils.hh"


MailBox *mailbox = 0;//mailbox engine
QPointer<QSplashScreen> splash;//single splash screen
MainWindow *main_window = 0;
alWizardFace *wizard_face = 0;
Constraints *constraints = 0;//constraints engine
int emit_locker = 0; //wrong emit protector


int main(int argc,char **argv)
{
    QApplication app(argc, argv);

    MainWindow mw;
    mw.show();
    main_window = &mw;
    int ret = app.exec();
    main_window = 0;
    return ret;
}
