#ifndef QT_BROWSER_MAIN_WINDOW_HH
#define QT_BROWSER_MAIN_WINDOW_HH

#include <QDialog>
#include <QMainWindow>
#include <QGridLayout>
#include <QEvent>
#include <QPointer>
#include <QLabel>

#include "help_browser.hh"
#include "connection.hh"
#include "constraints.hh"
#include "al_widget.hh"
#include "splashscreen.hh"

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
    void emitEvent(const QString& id,const QString& type, AlteratorRequestFlags request_flags = AlteratorRequestDefault);

signals:
    void languageChanged();

public slots:
    void start();
    void stop();

protected:
//    virtual void childEvent(QChildEvent*);
    virtual bool event(QEvent*);
    void timerEvent(QTimerEvent*);

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
    SplashScreen *splash;//single splash screen
    int busy_timer_id;
    QLabel *startup_splash;

    void keyPressEvent(QKeyEvent*);
//    void showEvent(QShowEvent*);
    void splashStart();
    void splashStop();
    void loadStyleSheet();

    void onRetryRequest();
    alWidget* onNewRequest(const AlteratorRequestActionAttrs &attr);
    void onCloseRequest(const QString& id);
    void onCleanRequest(const QString& id);
    void onSetRequest(const QString& id,const QString& attr,const QString& value);
    void onStartRequest(const QString& id);
    void onStopRequest(const QString& id);
    void onEventRequest(const QString& id,const QString& value);
    void onMessageBoxRequest(const QString& type, const QString& title,  const QString& message, QMessageBox::StandardButtons buttons);
    void onSplashMessageRequest(const QString& msg);

private slots:
    void onStartBusySplash();
    void onStopBusySplash();
    void onInternalSplashMessage(const QString& msg);

    void doRetry();
    void onAlteratorRequest(const AlteratorRequest&);
};
#endif
