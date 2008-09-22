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


typedef QMainWindow Browser_t;

class Browser: public Browser_t
{
    Q_OBJECT
public:
    Browser();
    ~Browser();

    bool haveWindowManager();
    void setFullScreen(bool);
    void setHelpSource(const QString&);
    void setHelpAvailable(bool);
    void changeLanguage(const QString&);
    void getDocument(const QString& request);
    void emitEvent(const QString &id,const QString &type, const AlteratorRequestFlags);
    void emitEvent(const QString &id, const BrowserEventType type, const AlteratorRequestFlags);

signals:
    void languageChanged();

public slots:
    void start();
    void stop();
    void quitApp();
    void quitAppWarn();
    void about();

protected:
    virtual bool event(QEvent*);
    void timerEvent(QTimerEvent*);
    void keyPressEvent(QKeyEvent*);

private:
    Constraints *constraints;
    Connection *connection;
    QTranslator *qtranslator;
    QTranslator *app_translator;
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
    bool help_available;

    void splashStart();
    void splashStop();
    void loadStyleSheet();
    static void collectTabIndex(QList<QString>&, QMap<QString, QMap<int,QWidget*> >&, alWidget* wdg, int);
    void reloadTranslator(QTranslator* translator, const QString &domain);

    void onRetryRequest();
    alWidget* onNewRequest(const AlteratorRequestActionAttrs &attr);
    void onCloseRequest(const QString& id);
    void onCleanRequest(const QString& id);
    void onSetRequest(const QString& id,const QString& attr,const QString& value);
    void onStartRequest(const QString& id);
    void onStopRequest(const QString& id);
    void onEventRequest(const QString& id,const QString& value);
    void onSplashMessageRequest(const QString& msg);
    void onMessageBoxRequest(const QString& type, const QString& title,  const QString& message, QMessageBox::StandardButtons buttons);
    void onFileSelectRequest(const QString& title, const QString& dir, const QString& type, const QString& mask);

private slots:
    void onStartBusySplash();
    void onCheckBusySplash();
    void onStopBusySplash();
    void onInternalSplashMessage(const QString& msg);

    void doRetry();
    void onAlteratorRequest(const AlteratorRequest&);
};

extern Browser *browser;

#endif
