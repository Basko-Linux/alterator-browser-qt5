#ifndef QTBROWSER_AL_MAILBOX_HH
#define QTBROWSER_AL_MAILBOX_HH

#include <sys/un.h>

#include <QLocalServer>
#include <QLocalSocket>
#include <QSocketNotifier>

#include "connection.hh"


class MailBox: public QLocalServer
{
Q_OBJECT
public:
	MailBox(const QString& addr, QObject *parent = 0);
	~MailBox();
public slots:
	void onNewConnection();
};

#endif
