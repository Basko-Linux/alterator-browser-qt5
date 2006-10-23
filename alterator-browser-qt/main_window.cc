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
//#include <sys/types.h>
#include <sys/wait.h>

#include "browser.hh"
#include "connection.hh"
#include "main_window.hh"
#include "updater.hh"

extern Updater *updater;
bool x_redirect_error;
bool x_error_occurred;
XErrorEvent x_last_error_event;
int x_catchRedirectError(Display *dpy, XErrorEvent *event)
{
    x_redirect_error = true;
    x_last_error_event = *event;
    x_error_occurred = true;
    return 0;
}

MainWindow::MainWindow():
    MainWindow_t()
{
    started = false;
    detect_wm = false;
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
    else
	showFullScreen();

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(stop()));
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
    initConnection(getDocParser);
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
    if( !started )
	QTimer::singleShot(0, this, SLOT(start()));
}

bool MainWindow::haveWindowManager()
{
    if( detect_wm )
	return have_wm;
    bool have_wm = false;
#ifdef Q_WS_X11
    const QX11Info xinfo = x11Info();
    Display *xdisplay = xinfo.display();
    int screen_id = xinfo.appScreen();

    int status = -1;
    pid_t pid = fork();
    if( pid == 0 )
    {
	Window xroot = RootWindow(xdisplay, screen_id);
	x_redirect_error = false;
	XSetErrorHandler(x_catchRedirectError);
/*
	XSelectInput(xdisplay, xroot,
	    ColormapChangeMask | EnterWindowMask | PropertyChangeMask | 
	    SubstructureRedirectMask | KeyPressMask |
	    ButtonPressMask | ButtonReleaseMask);
*/
	XSelectInput(xdisplay, xroot,
	    ColormapChangeMask | EnterWindowMask | PropertyChangeMask | 
	    SubstructureRedirectMask);
	XSync(xdisplay, 0);
	if( x_redirect_error )
	    exit(0);
	else
	    exit(1);
    }
    else if( pid > 0 )
    {
	waitpid(pid, &status, 0);
    }
    else
        qDebug("failed to fork");
    if( status == 0 )
	have_wm = true;
    else
	qDebug("No Window Manager detected");
#endif
    detect_wm = true;
    return have_wm;
}
