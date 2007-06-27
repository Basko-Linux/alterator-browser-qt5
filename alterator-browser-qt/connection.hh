#ifndef QTBROWSER_CONNECTION_HH
#define QTBROWSER_CONNECTION_HH

#include <QDomDocument>
#include <QThread>
#include <QTimer>
#include <QMap>
#include <QMessageBox>

#include "sax.hh"
#include "messagebox.hh"

typedef void (*parserfunc)(alCommand*);

enum AlteratorRequestActionType
{
    AlteratorRequestUnknown = 0,
    AlteratorRequestNew = 1,
    AlteratorRequestClose = 2,
    AlteratorRequestClean = 3,
    AlteratorRequestSet = 4,
    AlteratorRequestEvent = 5,
    AlteratorRequestSplash = 6,
    AlteratorRequestStart = 7,
    AlteratorRequestStop = 8,
    AlteratorRequestMessage = 9,
    AlteratorRequestLanguage = 10,
    AlteratorRequestCnstrAdd = 11,
    AlteratorRequestCnstrClear = 12,
    AlteratorRequestCnstrApply = 13,
    AlteratorRequestRetry = 14
};

enum AlteratorRequestParamType
{
    AltReqParamUnknown = 0,
    AltReqParamWId = 1,
    AltReqParamWType = 2,
    AltReqParamWParentId = 3,
    AltReqParamWWidth = 4,
    AltReqParamWHeight = 5,
    AltReqParamWOrientation = 6,
    AltReqParamWSubType = 7,
    AltReqParamWChecked = 8,
    AltReqParamWColumns = 9,
    AltReqParamWRowSpan = 10,
    AltReqParamWColSpan = 11,
    AltReqParamWTabIndex = 12,
    AltReqParamWAttrName = 13,
    AltReqParamWAttrValue = 14,
    AltReqParamEventValue = 15,
    AltReqParamSplashMessage = 16,
    AltReqParamMessageTitle = 17,
    AltReqParamMessageType = 18,
    AltReqParamMessage = 19,
    AltReqParamLanguage = 20,
    AltReqParamButtons = 21,
    AltReqParamCnstrName = 22,
    AltReqParamCnstrType = 23,
    AltReqParamCnstrParams = 24
};

enum AlteratorRequestParamDataType
{
    AltReqParamDataUnknown = 0,
    AltReqParamDataString = 1,
    AltReqParamDataBool = 2,
    AltReqParamDataInt = 3,
    AltReqParamDataOrientation = 4,
    AltReqParamDataButtons = 5
};

class AlteratorRequestParamData
{
public:
    AlteratorRequestParamData():
	type(AltReqParamDataUnknown),
	s(""),
	b(false),
	i(0),
	o(Qt::Vertical),
	buttons(QMessageBox::NoButton)
    {};
    ~AlteratorRequestParamData() {};

    AlteratorRequestParamDataType type;
    QString s;
    bool b;
    int i;
    Qt::Orientation o;
    QMessageBox::StandardButtons buttons;
};

typedef int AlteratorRequestFlags;
enum AlteratorRequestFlag
{
    AlteratorRequestDefault = 0,
    AlteratorRequestBlocking = 1,
    AlteratorRequestInit = 2
};

typedef QMap<AlteratorRequestParamType, AlteratorRequestParamData> AlteratorRequestActionAttrs;
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

class Connection: public QThread
{
    Q_OBJECT
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
    QMap<QString,AlteratorRequestActionType> str2action;
    AlteratorAskList requests;
    QString sessionId;
    QString userId;
    int islong_timer_id;
    bool destruction;

    AlteratorRequestParamData makeRequestParamData(AlteratorRequestParamDataType type, const QString& str);
    AlteratorRequestAction getDocParser(alCommand *cmd);
    QString makeRequest(const QString& content);
    QString makeInitRequest();
    void parseAnswer(alRequest *dom,
	AlteratorRequestFlags request_flags = AlteratorRequestDefault);
    QString createLangList();
    void run();
};

#endif
