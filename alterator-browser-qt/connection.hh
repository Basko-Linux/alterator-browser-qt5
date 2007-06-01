#ifndef QTBROWSER_CONNECTION_HH
#define QTBROWSER_CONNECTION_HH

#include <QDomDocument>
#include <QThread>
#include <QTimer>
#include <QMap>

#include "sax.hh"

typedef void (*parserfunc)(alCommand*);

enum AlteratorRequestAction
{
    AlteratorRequestUnknown,
    AlteratorRequestNew,
    AlteratorRequestClose,
    AlteratorRequestClean,
    AlteratorRequestSet,
    AlteratorRequestEvent,
    AlteratorRequestSplash,
    AlteratorRequestStart,
    AlteratorRequestStop,
    AlteratorRequestMessage,
    AlteratorRequestLanguage,
    AlteratorRequestCnstrAdd,
    AlteratorRequestCnstrClear,
    AlteratorRequestCnstrApply,
    AlteratorRequestRetry
};

typedef int AlteratorRequestFlags;
enum AlteratorRequestFlag
{
    AlteratorRequestDefault = 0,
    AlteratorRequestBlocking = 1,
    AlteratorRequestStartup = 2
};

struct AlteratorRequestActionInfo
{
    AlteratorRequestAction action;
    QMap<QString, QString> attr;
};

typedef QList<AlteratorRequestActionInfo> AlteratorRequestActionList;

struct AlteratorRequest
{
    AlteratorRequestFlags flags;
    AlteratorRequestActionList actions;
};

struct AlteratorAskInfo
{
    AlteratorRequestFlags flags;
    QString request;
};

typedef QList<AlteratorAskInfo> AlteratorAskList;

class Connection: public QThread
{
    Q_OBJECT;
public:
    Connection(QObject *parent = 0);
    ~Connection();

    void init();
    void getDocument(const QString& content = "(alterator-request action \"get\")",
	AlteratorRequestFlags request_flags = AlteratorRequestDefault);

signals:
    void alteratorRequest(const AlteratorRequest&);
    void startLongRequest();
    void stopLongRequest();

protected:
    void timerEvent(QTimerEvent*);

private slots:
    void startDelayedFinish();
    void endDelayedFinish();

private:
    QMap<QString,AlteratorRequestAction> str2action;
    AlteratorAskList requests;
    QString sessionId;
    QString userId;
    int islong_timer_id;
    bool destruction;

    AlteratorRequestActionInfo getDocParser(alCommand *cmd);
    QString makeRequest(const QString& content);
    void parseAnswer(alRequest *dom,
	AlteratorRequestFlags request_flags = AlteratorRequestDefault);
    QString createLangList();
    void run();
};

#endif
