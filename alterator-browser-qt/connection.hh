#ifndef QTBROWSER_CONNECTION_HH
#define QTBROWSER_CONNECTION_HH

#include <QDomDocument>
#include <QThread>
#include <QTimer>

#include "sax.hh"

typedef void (*parserfunc)(alCommand*);

class Connection: public QThread
{
    Q_OBJECT;
public:
    Connection(QObject *parent = 0);
    ~Connection();

    void init();
    void getDocument(const QString& content = "(alterator-request action \"get\")");

signals:
    void newRequest(const QMap<QString,QString>&);
    void closeRequest(const QString&);
    void cleanRequest(const QString&);
    void setRequest(const QString&, const QString&, const QString&);
    void eventRequest(const QString&, const QString&);
    void splashMessageRequest(const QString&);
    void startRequest(const QString&);
    void stopRequest(const QString&);
    void messageBoxRequest(const QString&, const QString&, const QString&, const QString&);
    void changeLanguageRequest(const QString&);
    void constraintsClearRequest();
    void constraintsApplyRequest();
    void constraintsAddRequest(const QString&, const QString&, const QString&);
    void retryRequest();
    void startLongRequest();
    void stopLongRequest();

protected:
    void timerEvent(QTimerEvent*);

private slots:
    void startDelayedFinish();
    void endDelayedFinish();

private:
    QStringList requests;
    QString sessionId;
    QString userId;
    int islong_timer_id;
    bool destruction;

    void getDocParser(alCommand *cmd);
    QString makeRequest(const QString& content);
    void parseAnswer(alRequest *dom);
    QString createLangList();
    void run();
};

#endif
