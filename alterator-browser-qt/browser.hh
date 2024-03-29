#ifndef QT_BROWSER_MAIN_WINDOW_HH
#define QT_BROWSER_MAIN_WINDOW_HH

#include <QDialog>
#include <QMainWindow>
#include <QStackedLayout>
#include <QEvent>
#include <QPointer>
#include <QLabel>
#include <QTranslator>

#include "help_browser.hh"
#include "connection.hh"
#include "al_widget.hh"
#include "splashscreen.hh"
#include "3dparty/waitingspinnerwidget.h"


typedef QMainWindow BrowserBase;

class Browser: public BrowserBase
{
    Q_OBJECT
public:
    Browser();
    ~Browser();

    enum PopupAddType
    {
	PopupDialog = 0,
	PopupMainNormalScreen = 1,
	PopupMainWideScreen = 2
    };

    bool haveWindowManager();
    void setFullScreen(bool);
    void setHelpSource(const QString&);
    void setHelpAvailable(bool);
    void changeLanguage(const QString&);
    void getDocument(const QString& request);
    void emitEvent(const QString &id,const QByteArray &type, const AlteratorRequestFlags);
    void emitEvent(const QString &id, const BrowserEventType type, const AlteratorRequestFlags);
    void popupAdd(QWidget *pop, PopupAddType popup_type = PopupDialog);
    void quitAppManaged(int);
    bool quitApp(int answ = QDialogButtonBox::NoButton);
    void quitAppError(const QString&);
    void raiseBrowserWindow();
    QString shortLang();
    static void unixSignalHandler(int);
    static QString createTmpDir(const QString& = QString());

Q_SIGNALS:
    void languageChanged();

public Q_SLOTS:
    void start();
    void stop();
    bool quitAppAsk();
    void about();
    void popupRemove(QWidget*);
    void onUnixSignal(int);
    void showHelp();
    void onStartBusySplash();
    void onCheckBusySplash();
    void onStopBusySplash();
    void onSplashMessageRequest(const QString& msg);
    void takeScreenShot(QWidget *wnd2 = nullptr);

protected:
    virtual void keyPressEvent(QKeyEvent*);
    virtual void closeEvent(QCloseEvent*);

private:
    QColor m_color_window_default;
    QString m_shortlang;
    Connection *connection;
    QTranslator *m_qt_translator;
    QTranslator *m_app_translator;
    QRect geometry_;
    bool m_first_screenshot = true;
    bool started;
    bool in_quit;
    bool have_wm;
    bool detect_wm_done;
    HelpBrowser *help_browser;
    bool internal_splash;
    bool alterator_splash;
    int emit_locker;
    SplashScreen *splash;//single splash screen
    QTimer *busy_timer;
    WaitingSpinnerWidget *startup_splash;
    bool help_available;
    QWidget *central_widget;
    QStackedLayout *central_layout;
    QMap<QWidget*,QWidget*> popups;

    void splashStart();
    void splashStop();
    void loadBranding();
    static void collectTabIndex(QList<QString>&, QMap<QString, QMap<int,QWidget*> >&, alWidget* wdg, int);
    void reloadTranslator(QTranslator** translator, const QString &domain, const QString &language);

    void onRetryRequest();
    alWidget* onNewRequest(const AlteratorRequestActionAttrs &attr);
    void onCloseRequest(const QString& id);
    void onCleanRequest(const QString& id);
    void onSetRequest(const QString& id,const QString& attr,const QString& value);
    void onStartRequest(const QString& id);
    void onStopRequest(const QString& id);
    void onEventRequest(const QString& id,const QString& value);
    void onMessageBoxRequest(const QString& type, const QString& title,  const QString& message, QDialogButtonBox::StandardButtons buttons);
    void onFileSelectRequest(const QString& title, const QString& dir, const QString& type, const QString& mask);

private Q_SLOTS:
    void onInternalSplashMessage(const QString& msg);

    void doRetry();
    void onAlteratorRequests();
    void onAlteratorRequest(const AlteratorRequest&);
};

extern Browser *g_browser;

#endif
