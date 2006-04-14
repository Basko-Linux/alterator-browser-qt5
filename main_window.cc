
#include <QTextStream>
#include <QDataStream>
#include <QTimer>
#include <QCursor>
#include <QSettings>
#include <QApplication>
#include <QDesktopWidget>
#ifdef Q_WS_X11
#include <QX11Info>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#endif

#include "browser.hh"
#include "connection.hh"
#include "main_window.hh"
#include "updater.hh"

extern Updater *updater;
extern APixmaps *pixmaps;

MainWindow::MainWindow():
    MainWindow_t()
{
    started = false;
    have_wm = haveWindowManager();
    if( have_wm )
    {
	QRect desktop_geom  = QApplication::desktop()->geometry();
	int wnd_recom_width = (int)(desktop_geom.width()/1.5);
	int wnd_recom_height = (int)(desktop_geom.height()/1.5);

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "alterator", "browser", this);
	settings.setFallbacksEnabled(false);
	settings.beginGroup("qt");
	int wnd_x = settings.value("main_window_x", 0).toInt();
	int wnd_y = settings.value("main_window_y", 0).toInt();
	int wnd_width = settings.value("main_window_width", wnd_recom_width).toInt();
	int wnd_height = settings.value("main_window_height", wnd_recom_height).toInt();
	settings.endGroup();

	setGeometry(wnd_x, wnd_y, wnd_width, wnd_height);
    }
#if 0 //temporary disable
    else
	setWindowState(windowState() | Qt::WindowFullScreen);
#endif

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(stop()));
}

MainWindow::~MainWindow()
{
    if( updater )
    {
	delete updater;
	updater = 0;
    }
    if( pixmaps )
    {
	delete pixmaps;
	pixmaps = 0;
    }
}

void MainWindow::start()
{
    if( started ) return;
    started = true;

    if( !pixmaps )
	pixmaps = new APixmaps();
    updater = new Updater();
#if 1
    initConnection(getDocParser);
#endif
}

void MainWindow::stop()
{
    if( have_wm )
    {
	QRect geom = geometry();

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "alterator", "browser", this);
	settings.setFallbacksEnabled(false);
	settings.beginGroup("qt");
	settings.setValue("main_window_x", geom.x());
	settings.setValue("main_window_y", geom.y());
	settings.setValue("main_window_width", geom.width());
	settings.setValue("main_window_height", geom.height());
	settings.endGroup();
    }
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
