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
    Q_EMIT newConnection(fd);
}


MailBox::MailBox(const QString& path, QObject *parent):
	MbLocalServer(parent)
{
    if( QFile::exists(path) )
	QFile::remove(path);

    setMaxPendingConnections(8);
    if( !listen(path) )
    {
	g_browser->quitAppError(errorString());
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

	MbSocketNotifier *notifier = new MbSocketNotifier(fd, QSocketNotifier::Read, localsock);
	connect(notifier, SIGNAL(activated(int)), this, SLOT(readMessage(int)));
	connect(localsock, SIGNAL(disconnected()), notifier, SLOT(disable()));
	connect(localsock, SIGNAL(disconnected()), localsock, SLOT(deleteLater()));
}

void MailBox::readMessage(int fd)
{
    FILE *fl = fdopen(fd, "r");
    if( fl )
    {
	QString msgremain;
	QTextStream fs(fl,QIODevice::ReadOnly);
	fs.setCodec("UTF-8");
	do {
	    QString msgdata(msgremain);
	    msgdata.append(fs.readLine(1024));
	    QStringList msglist(msgdata.split(QChar('\0'),Qt::KeepEmptyParts));
	    QStringListIterator is(msglist);
	    while( is.hasNext() )
	    {
		QString message = is.next();
		if(!message.isEmpty())
		{
		    //qDebug("MailBox::readMessage mailbox message:%s",qPrintable(message));
		    if( is.hasNext() || fs.atEnd() )
		    {
			g_browser->getDocument(QString("(mailbox-request %1 )").arg(message));
			msgremain.clear();
		    }
		    else
		    {
			msgremain = message;
		    }
		    //qDebug("MailBox::readMessage end of processing....");
		}
	    }
	} while( !fs.atEnd() );
    }
}
