#ifndef QTBROWSER_AL_MAILBOX_HH
#define QTBROWSER_AL_MAILBOX_HH

#include <sys/un.h>

#include <QObject>
#include <QString>
#include <QSocketNotifier>

#include "connection.hh"


class MailBox: public QObject
{
Q_OBJECT
public:
	MailBox(const QString& addr, parserfunc parser, QObject *parent = 0);
	~MailBox();
public slots:
	void onMessage(int);
	void readMessage(int);
	void doRetry();
private:
	parserfunc parser_;
	int sock_;
	sockaddr_un socka_;
	int size_;
	QSocketNotifier *notifier_;
	QSocketNotifier *eater_;
};


#endif
