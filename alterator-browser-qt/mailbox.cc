
#include <QLocalSocket>
#include <QSocketNotifier>

#include "utils.hh"
#include "main_window.hh"

#include "mailbox.hh"

using namespace Utils;

extern MainWindow *main_window;

MailBox::MailBox(const QString& path, QObject *parent):
	QLocalServer(parent)
{
    QFile::remove(path);
    if( !listen(path) )
    {
	errorMessage(QString("Unable to listen on local socket <%1>").arg(path));
    }

    connect(this, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
}

MailBox::~MailBox()
{
}

void MailBox::onNewConnection()
{
    while(hasPendingConnections())
    {
	QLocalSocket *local_sock = nextPendingConnection();
	int fd = local_sock->socketDescriptor();
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
	delete local_sock;
    }
}
