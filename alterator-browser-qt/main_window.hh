#ifndef QT_BROWSER_MAIN_WINDOW_HH
#define QT_BROWSER_MAIN_WINDOW_HH

#include <QDialog>
#include <QMainWindow>
#include <QGridLayout>
#include <QEvent>
#include <QSplashScreen>
#include <QPointer>

#include "help_browser.hh"
#include "connection.hh"
#include "constraints.hh"

typedef QMainWindow MainWindow_t;

class MainWindow: public MainWindow_t
{
    Q_OBJECT
public:
    MainWindow();
    ~MainWindow();

    bool haveWindowManager();
    void setFullScreen(bool);
    void setHelpSource(const QString&);
    void changeLanguage(const QString&);
    void getDocument(const QString& request);
    void emitEvent(const QString& id,const QString& type);

signals:
    void languageChanged();

public slots:
    void start();
    void stop();

protected:
//    virtual void childEvent(QChildEvent*);
    virtual bool event(QEvent*);

private:
    Constraints *constraints;
    Connection *connection;
    QTranslator *qtranslator;
    QRect geometry_;
    bool started;
    bool have_wm;
    bool detect_wm;
    HelpBrowser *help_browser;
    bool internal_splash;
    bool alterator_splash;
    int emit_locker;
    QPointer<QSplashScreen> splash;//single splash screen
    QTimer *busy_timer;

    void keyPressEvent(QKeyEvent*);
//    void showEvent(QShowEvent*);
    void customEvent(QEvent*);
    void splashStart();

private slots:
    void onStartBusySplash();
    void onStopBusySplash();
    void onStopBusy();
    void onInternalSplashMessage(const QString& msg);

    void doRetry();
    void onRetryRequest();
    void onNewRequest(const QXmlAttributes& attr);
    void onCloseRequest(const QString& id);
    void onCleanRequest(const QString& id);
    void onSetRequest(const QString& id,const QString& attr,const QString& value);
    void onStartRequest(const QString& id);
    void onStopRequest(const QString& id);
    void onEventRequest(const QString& id,const QString& value);
    void onMessageBoxRequest(const QXmlAttributes& e);
    void onSplashMessageRequest(const QString& msg);
};
#endif
