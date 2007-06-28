#ifndef QTBROWSER_CONNECTION_HH
#define QTBROWSER_CONNECTION_HH

#include <QDomDocument>
#include <QThread>
#include <QTimer>
#include <QMap>
#include <QMessageBox>

#include "sax.hh"
#include "messagebox.hh"
#include "enums.hh"

typedef void (*parserfunc)(alCommand*);

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
