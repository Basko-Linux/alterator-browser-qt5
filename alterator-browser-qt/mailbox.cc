#include "mailbox.hh"

#include "utils.hh"
#include "browser.hh"

using namespace Utils;

MbSocketNotifier::MbSocketNotifier(int socket, QSocketNotifier::Type tp, QObject *parent):
    QSocketNotifier(socket, tp, parent)
{
}

MbSocketNotifier::~MbSocketNotifier()
{
}

void MbSocketNotifier::enable()
{
    setEnabled(true);
}

void MbSocketNotifier::disable()
{
    setEnabled(false);
}


MbLocalServer::MbLocalServer(QObject *parent):
    QLocalServer(parent)
{}

MbLocalServer::~MbLocalServer()
{}

void MbLocalServer::incomingConnection(quintptr fd)
{
    emit newConnection(fd);
}


MailBox::MailBox(const QString& path, QObject *parent):
	MbLocalServer(parent)
{
    if( QFile::exists(path) )
	QFile::remove(path);

    setMaxPendingConnections(8);
    if( !listen(path) )
    {
	browser->quitAppError(errorString());
    }

    QObject::connect(this, SIGNAL(newConnection(int)), this, SLOT(onNewConnection(int)));
}

MailBox::~MailBox()
{
}

void MailBox::onNewConnection(int fd)
{
	readMessage(fd);

	QLocalSocket *localsock = new QLocalSocket(this);
	localsock->setSocketDescriptor(fd, QLocalSocket::ConnectedState, QIODevice::ReadOnly);

	bool has_notifier = false;
	foreach(QObject *child, localsock->children())
	{
	    if( QSocketNotifier *notifier = qobject_cast<QSocketNotifier*>(child) )
	    {
	        notifier->setEnabled(true);
	        has_notifier = true;
	    }
	}
	if( !has_notifier )
	{
	    MbSocketNotifier *notifier = new MbSocketNotifier(fd, QSocketNotifier::Read, localsock);
	    connect(notifier, SIGNAL(activated(int)), this, SLOT(readMessage(int)));
	    connect(localsock, SIGNAL(disconnected()), notifier, SLOT(disable()));
	}
	connect(localsock, SIGNAL(disconnected()), localsock, SLOT(deleteLater()));
}

void MailBox::readMessage(int fd)
{
	QString message;
	char ch = 0;
	int len = 0;
	while ((len = read(fd,&ch,1)) > 0)
	{
	    if(ch == '\0')
	    {
		if(!message.isEmpty())
		{
		    //qDebug("MailBox::readMessage mailbox message:%s",qPrintable(message));
		    browser->getDocument(QString("(mailbox-request %1 )").arg(message));
		    //qDebug("MailBox::readMessage end of processing....");

		    message = "";
		}
	    }
	    else
		message += ch;
	}

	if(!message.isEmpty())
	{
	    //qDebug("MailBox::readMessage mailbox message:%s",qPrintable(message));
	    browser->getDocument(QString("(mailbox-request %1 )").arg(message));
	    //qDebug("MailBox::readMessage end of processing....");
	}
}
