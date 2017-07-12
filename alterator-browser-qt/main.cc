#include <unistd.h>
#include <csignal>

#include <libudev.h>

#include <QApplication>
#include "browser.hh"
#include "enums.hh"

Browser *g_browser = 0;
Enums *g_enums = 0;

int main(int argc,char **argv)
{
    bool is_running = false;
    int ret = 0;

    QString tmpdir( Browser::createTmpDir() );
    if( tmpdir.isEmpty() )
    {
	qFatal("Quit alterator-browser");
    }

    QString pidfile_path = QString("%1/alterator-browser-x11-%2.pid").arg(tmpdir).arg(getuid());
    { // check browser is running
	QFile pidfile(pidfile_path);
	if( pidfile.open(QIODevice::ReadOnly) )
	{
	    char pid_buf[1024];
	    qint64 line_len = pidfile.readLine(pid_buf, sizeof(pid_buf));
	    if( line_len > 0 )
	    {
		int pid = QString::fromLatin1(pid_buf).trimmed().toInt();
		if(pid > 0)
		{
		    QString cmdline_path = QString("/proc/%1/cmdline").arg(pid);
		    QFile cmdline_file(cmdline_path);
		    if(cmdline_file.open(QIODevice::ReadOnly))
		    {
	    		char cmd_buf[1024];
	    		qint64 line_len = cmdline_file.readLine(cmd_buf, sizeof(cmd_buf));
	    		if( line_len > 0 )
	    		{
	    		    if( QString::fromLatin1(cmd_buf).trimmed().contains("alterator-browser-") )
	    		    {
				//qWarning("alterator-browser is already running at pid %d", pid);
	    			kill(pid, SIGUSR1);
	    			is_running = true;
	    		    }
	    		}
	    		cmdline_file.close();
	    	    }
		}
	    }
	    pidfile.close();
	}
    }

    if( !is_running )
    { // create pid file
	QFile pidfile(pidfile_path);
	if( pidfile.open(QIODevice::WriteOnly|QIODevice::Truncate) )
	{
	    int num_hw_keyboards = -1;
	    {
	        struct udev *udev;
	        struct udev_enumerate *enumerate;
	        struct udev_list_entry *devices, *dev_list_entry;
	        struct udev_device *dev;

		udev = udev_new();
		if( udev ) {
		    num_hw_keyboards = 0;
		    enumerate = udev_enumerate_new(udev);
		    udev_enumerate_add_match_property(enumerate, "ID_INPUT_KEYBOARD", "1");
		    udev_enumerate_scan_devices(enumerate);
		    devices = udev_enumerate_get_list_entry(enumerate);
		    udev_list_entry_foreach(dev_list_entry, devices) {
			const char *path, *devnode;
			path = udev_list_entry_get_name(dev_list_entry);
			dev = udev_device_new_from_syspath(udev, path);
			devnode = udev_device_get_devnode(dev);
			if (devnode) {
			    num_hw_keyboards++;
			}
			udev_device_unref(dev);
		    }
		    udev_enumerate_unref(enumerate);
		    udev_unref(udev);
		} else {
		    qWarning("Can't create udev handle.");
		}
	    }
	    if( num_hw_keyboards == 0 ) {
		// try to use virtual keyboard
		qputenv("QT_IM_MODULE", "qtvirtualkeyboard");
	    }

	    QString write_line = QString("%1").arg(getpid());
	    pidfile.write(write_line.toLocal8Bit());
	    pidfile.close();

	    Q_INIT_RESOURCE(images);
	    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	    QApplication app(argc, argv);

	    Enums enums_;
	    g_enums = &enums_;

	    Browser mw;
	    mw.show();
	    g_browser = &mw;
#if 0
	    app.watchUnixSignal(SIGUSR1, true);
	    QObject::connect(QCoreApplication::instance(), SIGNAL(unixSignal(int)), g_browser, SLOT(onUnixSignal(int)));
#else
            struct sigaction sa;
            sigemptyset(&(sa.sa_mask));
            sa.sa_flags = 0;
            sa.sa_handler = Browser::unixSignalHandler;
            sigaction(SIGUSR1, &sa, 0);
#endif

	    ret = app.exec();
	    g_browser = 0;
	    g_enums = 0;
	}
	else
	{
	    qFatal("Unable to create pidfile %s", qPrintable(pidfile_path));
	}
    }

    return ret;
}
