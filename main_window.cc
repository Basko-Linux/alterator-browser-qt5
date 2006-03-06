
#include <QTextStream>
#include <QDataStream>
#include <QTimer>
#include <QGridLayout>
#include <QCursor>
#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

#include "browser.hh"
#include "connection.hh"
#include "main_window.hh"

extern Updater *updater;

MainWindow::MainWindow():
    MainWindow_t()
{
    started = false;
    have_wm = haveWindowManager();
    if( !have_wm )
	setWindowState(windowState() | Qt::WindowFullScreen);
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
    if( started ) return;
    started = true;
    updater = new Updater();
#if 1
    initConnection(getDocParser);
#endif
}

void MainWindow::showEvent(QShowEvent*)
{
    QTimer::singleShot(0, this, SLOT(start()));
}

bool MainWindow::haveWindowManager()
{
    bool have_wm = false;
#ifdef Q_WS_X11
    const QX11Info xinfo = x11Info();
    Display *xdisplay = xinfo.display();
    int screen_id = xinfo.appScreen();

    Atom type_ret;
    int format_ret;
    unsigned char *data_ret = 0;
    unsigned long nitems_ret, unused;

    Window xroot = RootWindow(xdisplay, screen_id);

    Atom supporting_atom = XInternAtom (xdisplay, "_NET_SUPPORTING_WM_CHECK", False);
    Atom wm_name_atom = XInternAtom (xdisplay, "_NET_WM_NAME", False);
    Atom UTF8_STRING = XInternAtom (xdisplay, "UTF8_STRING", False);

    if (XGetWindowProperty(xdisplay, xroot, supporting_atom,
	0l, 1l, False,
	XA_WINDOW, &type_ret, &format_ret,
	&nitems_ret, &unused, &data_ret) == Success)
    {
	if (data_ret)
	{
	    Window supportwindow = *((Window *) data_ret);
	    unsigned char *name_ret = 0;
	    if (XGetWindowProperty(xdisplay, supportwindow, wm_name_atom,
		0l, (long) 4096, False,
		UTF8_STRING, &type_ret, &format_ret,
		&nitems_ret, &unused, &name_ret)	== Success)
		{
		    if ( name_ret )
		    {
		        have_wm = true;
		        XFree(name_ret);
		    }
		}
	    XFree(data_ret);
	}
    }
#endif
    return have_wm;
}
