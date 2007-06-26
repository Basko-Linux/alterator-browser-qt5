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
    AlteratorRequestParamUnknown = 0,
    AlteratorRequestParamWidgetId = 1,
    AlteratorRequestParamWidgetType = 2,
    AlteratorRequestParamParentId = 3,
    AlteratorRequestParamWidth = 4,
    AlteratorRequestParamHeight = 5,
    AlteratorRequestParamOrientation = 6,
    AlteratorRequestParamSubType = 7,
    AlteratorRequestParamChecked = 8,
    AlteratorRequestParamColumns = 9,
    AlteratorRequestParamTabIndex = 10,
    AlteratorRequestParamWidgetAttrName = 11,
    AlteratorRequestParamWidgetAttrValue = 12,
    AlteratorRequestParamEventValue = 12,
    AlteratorRequestParamSplashMessage = 13,
    AlteratorRequestParamMessageTitle = 14,
    AlteratorRequestParamMessageType = 15,
    AlteratorRequestParamMessage = 16,
    AlteratorRequestParamLanguage = 17,
    AlteratorRequestParamButtons = 18,
    AlteratorRequestParamCnstrName = 19,
    AlteratorRequestParamCnstrType = 20,
    AlteratorRequestParamCnstrParams = 21
};

enum AlteratorRequestParamDataType
{
    AlteratorRequestParamDataUnknown = 0,
    AlteratorRequestParamDataString = 1,
    AlteratorRequestParamDataBool = 2,
    AlteratorRequestParamDataInt = 3,
    AlteratorRequestParamDataOrientation = 4,
    AlteratorRequestParamDataButtons = 5
};

class AlteratorRequestParamData
{
public:
    AlteratorRequestParamData():
	type(AlteratorRequestParamDataUnknown),
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
