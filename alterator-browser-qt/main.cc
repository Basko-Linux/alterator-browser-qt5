#include <csignal>

#include <QApplication>
#include "browser.hh"
#include "enums.hh"

Browser *browser = 0;
Enums *enums = 0;

int main(int argc,char **argv)
{
    bool is_running = false;
    int ret = 0;
    QString tmpdir(getenv("TMPDIR"));
    if( tmpdir.isEmpty() )
	tmpdir = "/tmp";
    QString pidfile_path = QString("%1/alterator-browser-qt-%2.pid").arg(getenv("TMPDIR")).arg(getuid());
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
	    QString write_line = QString("%1").arg(getpid());
	    pidfile.write(write_line.toLocal8Bit());
	    pidfile.close();

	    Q_INIT_RESOURCE(images);
	    QApplication app(argc, argv);
	    app.watchUnixSignal(SIGUSR1, true);

	    Enums enums_;
	    enums = &enums_;

	    Browser mw;
	    mw.show();
	    browser = &mw;
	    QObject::connect(QCoreApplication::instance(), SIGNAL(unixSignal(int)), browser, SLOT(onUnixSignal(int)));

	    ret = app.exec();
	    browser = 0;
	    enums = 0;
	}
	else
	{
	    qFatal("Unable to create pidfile %s", qPrintable(pidfile_path));
	}
    }

    return ret;
}
