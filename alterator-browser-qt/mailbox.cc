#include "mailbox.hh"

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include <QFile>

#include "utils.hh"
#include "main_window.hh"

using namespace Utils;

extern MainWindow *main_window;

MailBox::MailBox(const QString& path, QObject *parent):
	QObject(parent),
	eater_(0)
{
	if ((sock_ = ::socket(PF_UNIX, SOCK_STREAM, 0)) == -1)
		errorMessage("socket");

	::memset(socka_.sun_path,0,sizeof(socka_.sun_path)/sizeof(char));
	socka_.sun_family = AF_UNIX;
	::strncpy(socka_.sun_path,path.toLatin1().data(),sizeof(socka_.sun_path)/sizeof(char)-1);

	int size = SUN_LEN(&socka_);

	if (::bind(sock_, (struct sockaddr *)&socka_, size) == -1) {
	    if (::connect(sock_, (struct sockaddr*)&socka_, size) == -1) {
		    if (!QFile::remove(path))
		    	errorMessage("remove");

		    if (::bind(sock_, (struct sockaddr *)&socka_, size) == -1)
		    	errorMessage("re-bind");
	    }
	    else
	    	errorMessage("Address already in use");
	}

	if (::listen(sock_, 1) == -1)
		errorMessage("listen");

	notifier_ = new QSocketNotifier(sock_,QSocketNotifier::Read);
	QObject::connect(notifier_,SIGNAL(activated(int)),this, SLOT(onMessage(int)) );
}

MailBox::~MailBox()
{
	::close(sock_);
	delete notifier_;
}

void MailBox::onMessage(int)
{
	int fd = ::accept(sock_, NULL, NULL);
	
	if (fd == -1)
	{
		qDebug("invalid connection");
		return;
	}

	notifier_->setEnabled(false);
	eater_ = new QSocketNotifier(fd, QSocketNotifier::Read);
	connect(eater_, SIGNAL(activated(int)), this, SLOT(readMessage(int)));
	readMessage(fd);
}

void MailBox::readMessage(int fd)
{
	QString message = "";
	char ch;
	int len;
	while ((len = read(fd,&ch,1)) > 0)
	{
		if (!ch) break;
		message += ch;
	}
	
	if (!message.isEmpty())
	{
		//qDebug("mailbox message:%s",qPrintable(message));
		main_window->getDocument(QString("(mailbox-request %1 )").arg(message));
		//qDebug("end of processing....");
	}
	else if (len <= 0)
	{
		::close(fd);
		notifier_->setEnabled(true);
		delete eater_;
		eater_ = 0;
	}
}
