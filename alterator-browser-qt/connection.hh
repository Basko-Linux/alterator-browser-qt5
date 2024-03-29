#ifndef QTBROWSER_CONNECTION_HH
#define QTBROWSER_CONNECTION_HH

#include <QDomDocument>
#include <QThread>
#include <QTimer>
#include <QMap>
#include <QHash>
#include <QMessageBox>
#include <QQueue>
#include <QMutex>

#include "sax.hh"
#include "messagebox.hh"
#include "enums.hh"

typedef void (*parserfunc)(alCommand*);

enum AlteratorRequestParamDataType
{
    AltReqParamDataUnknown = 0,
    AltReqParamDataType = 1,
    AltReqParamDataString = 2,
    AltReqParamDataBool = 3,
    AltReqParamDataInt = 4,
    AltReqParamDataOrientation = 5,
    AltReqParamDataButtons = 6
};

class AlteratorRequestParamData
{
public:
    AlteratorRequestParamData():
	type(AltReqParamDataUnknown),
	t(WUnknown),
	s(QString()),
	b(false),
	i(0),
	o(Qt::Vertical),
	buttons(QMessageBox::NoButton)
    {};
    ~AlteratorRequestParamData() {};

    AlteratorRequestParamDataType type;
    AlteratorWidgetType t;
    QString s;
    bool b;
    int i;
    Qt::Orientation o;
    QDialogButtonBox::StandardButtons buttons;
};

typedef int AlteratorRequestFlags;
enum AlteratorRequestFlag
{
    AlteratorRequestDefault = 0,
    AlteratorRequestBlocking = 1,
    AlteratorRequestInit = 2,
    AlteratorRequestTimeReset = 4,
    AlteratorRequestCenterFaceModuleSelected = 8
};

typedef QHash<QString, AlteratorRequestParamData> AlteratorRequestActionAttrs;
struct AlteratorRequestAction
{
    AlteratorRequestActionType action;
    AlteratorRequestActionAttrs attr;
};

typedef QList<AlteratorRequestAction> AlteratorRequestActionList;

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

typedef QQueue<AlteratorAskInfo> AlteratorAskList;

class Connection: public QThread
{
    Q_OBJECT
public:
    Connection(QObject *parent = 0);
    ~Connection();

    void init();
    void getDocument(const QString& content = QStringLiteral("(alterator-request action \"get\")"),
	AlteratorRequestFlags request_flags = AlteratorRequestDefault);
    QQueue<AlteratorRequest> getRequests();
    void run();

Q_SIGNALS:
    void alteratorRequests();
    void startLongRequest();
    void stopLongRequest();
    void startProcessing();

private Q_SLOTS:
    void checkDelayedFinish();
    void prepareQuit();

private:
    AlteratorAskList asks;
    QQueue<AlteratorRequest> alterator_requests;
    QMutex asks_lock;
    QMutex alterator_requests_lock;
    QTimer *islong_timer;
    QString sessionId;
    QString userId;
    bool destruction;
    bool is_processing;

    AlteratorRequestParamData makeRequestParamData(AlteratorRequestParamDataType type, const QString& str);
    bool setRequestActionParamData(QXmlAttributes &xmlattrs, const QString &xmlattrname, AlteratorRequestAction &action, const QString &attrname, AlteratorRequestParamDataType dtype);
    void setRequestActionParamString(const QString &value, AlteratorRequestAction &action, const QString &attrname);
    AlteratorRequestAction getDocParser(alCommand *cmd);
    QString makeRequest(const QString& content);
    QString makeInitRequest();
    void parseAnswer(const alRequest &dom,
	AlteratorRequestFlags request_flags = AlteratorRequestDefault);
    QString createLangList();

#ifdef QTHREAD_EXEC_WORKAROUND
    bool thread_exec;
    int  thread_exec_result;
    int myExec();
    void myQuit();
    void myExit(int);
#endif
};

#endif
