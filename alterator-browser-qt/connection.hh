#ifndef QTBROWSER_CONNECTION_HH
#define QTBROWSER_CONNECTION_HH

#include <QDomDocument>
#include <QThread>
#include <QTimer>
#include <QMap>
#include <QHash>
#include <QMessageBox>

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
	s(""),
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

typedef QList<AlteratorAskInfo> AlteratorAskList;

#ifndef USE_CONN_NO_THREAD
class Connection: public QThread
#else
class Connection: public QObject
#endif
{
    Q_OBJECT
public:
    Connection(QObject *parent = 0);
    ~Connection();

#ifndef USE_CONN_NO_THREAD
#else
signals:
    void started();
    void finished();
public:
    bool is_running;
    bool isRunning() { return is_running; };
    void quit() { destruction = true; };
    void start() { run(); };
#endif

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
    void prepareQuit();

private:
    AlteratorAskList requests;
    QString sessionId;
    QString userId;
    int islong_timer_id;
    bool destruction;

    AlteratorRequestParamData makeRequestParamData(AlteratorRequestParamDataType type, const QString& str);
    void setRequestActionParamData(QXmlAttributes &xmlattrs, const QString &xmlattrname, AlteratorRequestAction &action, const QString &attrname, AlteratorRequestParamDataType dtype);
    AlteratorRequestAction getDocParser(alCommand *cmd);
    QString makeRequest(const QString& content);
    QString makeInitRequest();
    void parseAnswer(const alRequest &dom,
	AlteratorRequestFlags request_flags = AlteratorRequestDefault);
    QString createLangList();
    void run();
};

#endif
