#ifndef QTBROWSER_AL_MAILBOX_HH
#define QTBROWSER_AL_MAILBOX_HH

#include <QLocalServer>
#include <QSocketNotifier>
#include <QSignalMapper>
#include <QLocalSocket>

#include "connection.hh"

class MbSocketNotifier: public QSocketNotifier
{
Q_OBJECT
public:
    MbSocketNotifier(int socket, QSocketNotifier::Type, QObject *parent = 0);
    ~MbSocketNotifier();
public Q_SLOTS:
    void enable();
    void disable();
};

class MbLocalServer: public QLocalServer
{
Q_OBJECT
public:
    MbLocalServer(QObject *parent);
    ~MbLocalServer();
Q_SIGNALS:
    void newConnection(int);
protected:
    virtual void incomingConnection(quintptr);
};

class MailBox: public MbLocalServer
{
Q_OBJECT
public:
	MailBox(const QString& addr, QObject *parent = 0);
	~MailBox();

public Q_SLOTS:
	void onNewConnection(int);
	void readMessage(int);
};


#endif
