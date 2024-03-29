//#include <sys/types.h>
#include <sys/wait.h>

#include <QTextStream>
#include <QDataStream>
#include <QTimer>
#include <QCursor>
#include <QSettings>
#include <QGuiApplication>
#include <QDesktopWidget>
#include <QDir>
#include <QTranslator>
#include <QLocale>
#include <QLibraryInfo>
#include <QPainter>
#include <QStyleFactory>
#include <QResource>
#include <QMovie>
#include <QFileDialog>
#include <QScreen>

#include "global.hh"
#include "widgets.hh"
#include "connection.hh"
#include "browser.hh"
#include "messagebox.hh"
#include "mailbox.hh"
#include "widgetlist.hh"
#include "a_pixmaps.hh"
#include "enums.hh"
#include "fileselect.hh"

#ifdef HAVE_X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <QX11Info>
#endif

MailBox *mailbox = 0;
WidgetList *g_widgetlist = 0;

#ifdef HAVE_X11
bool x_redirect_error;
bool x_error_occurred;
XErrorEvent x_last_error_event;
int x_catchRedirectError(Display *, XErrorEvent *event)
{
    x_redirect_error = true;
    x_last_error_event = *event;
    x_error_occurred = true;
    return 0;
}
#endif

Browser::Browser():
    BrowserBase(0)
{
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    loadBranding();

    internal_splash = false;
    alterator_splash = false;
    splash = 0;
    emit_locker = 0; //wrong emit protector
    connection = 0;
    m_qt_translator = 0;
    m_app_translator = 0;
    started = false;
    in_quit = false;
    detect_wm_done = false;
    have_wm = false;
    help_available = true;
    central_widget = 0;
    central_layout = 0;
    m_color_window_default = palette().window().color();
    g_widgetlist = new WidgetList(this);
    pixmap_paths
	<< QStringLiteral(":/design/")
	<< QStringLiteral(":/images/")
	<< QStringLiteral("/usr/share/alterator/design/images/")
	<< QStringLiteral("/usr/share/alterator/images/");

    busy_timer = new QTimer(this);
    busy_timer->setSingleShot(true);
    busy_timer->setInterval(500);
    connect(busy_timer, SIGNAL(timeout()), this, SLOT(onStopBusySplash()));

    QString language(QLatin1String(qgetenv("LC_ALL")));
    if( language.isEmpty() )
	language = QLatin1String(qgetenv("LC_MESSAGES"));
    if( language.isEmpty() )
	language = QLatin1String(qgetenv("LANG"));
    if( language.isEmpty() )
	language = QStringLiteral("C");
    changeLanguage(language);

    window()->setWindowTitle(QStringLiteral("alterator"));

    central_widget = new QWidget(this);
    central_layout = new QStackedLayout(central_widget);
    central_layout->setStackingMode(QStackedLayout::StackAll);
    setCentralWidget(central_widget);

    if( haveWindowManager() )
    {
	QScreen* primary_screen = QGuiApplication::primaryScreen();
	QRect screen_geom  = primary_screen? primary_screen->geometry(): QRect(0,0,1280,720);
	int wnd_recom_width = (int)(screen_geom.width()/1.5);
	int wnd_recom_height = (int)(screen_geom.height()/1.5);

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, QStringLiteral("alterator"), QStringLiteral("browser"), this);
	settings.setFallbacksEnabled(false);
	settings.beginGroup(QStringLiteral("qt"));
	int wnd_x = settings.value(QStringLiteral("main_window_x"), 0).toInt();
	int wnd_y = settings.value(QStringLiteral("main_window_y"), 0).toInt();
	int wnd_width = settings.value(QStringLiteral("main_window_width"), wnd_recom_width).toInt();
	int wnd_height = settings.value(QStringLiteral("main_window_height"), wnd_recom_height).toInt();
	settings.endGroup();

	geometry_ = QRect(wnd_x, wnd_y, wnd_width, wnd_height);
	resize(wnd_width,wnd_height);
    }
    else
    {
	setFullScreen(true);
	QPalette pal = palette();
	pal.setColor(QPalette::Window, QColor(0,0,0));
	setPalette(pal);
    }

    // startup animation
    startup_splash = new WaitingSpinnerWidget(this, true, false);
    {
	int unit = qMax(font().pixelSize(), font().pointSize()) * 3;
	startup_splash->setRoundness(unit*2);
	startup_splash->setMinimumTrailOpacity(0.0);
	startup_splash->setTrailFadePercentage(85.0);
	startup_splash->setNumberOfLines(16);
	startup_splash->setLineLength(unit);
	startup_splash->setLineWidth(unit/2);
	startup_splash->setInnerRadius(unit);
	startup_splash->setRevolutionsPerSecond(1);
	startup_splash->setColor(palette().highlight().color());
	startup_splash->setCursor(Qt::BlankCursor);
    }
    central_layout->addWidget(startup_splash);
    startup_splash->start();

    help_browser = new HelpBrowser(this);

    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(stop()));
    QTimer::singleShot(0, this, SLOT(start()));
}

Browser::~Browser()
{
}

QString Browser::createTmpDir(const QString& tmp)
{
    QString tmpdir_path(tmp.isEmpty()? QLatin1String(qgetenv("TMPDIR")): tmp);
    if( tmpdir_path.isEmpty() )
	tmpdir_path = QStringLiteral("/tmp");
    QDir tmpdir(tmpdir_path);
    tmpdir_path.append(QStringLiteral("/alterator"));
    if( (tmpdir.exists(QStringLiteral("alterator")) && QFileInfo(tmpdir_path).isDir()) || tmpdir.mkdir(QStringLiteral("alterator")) )
	return tmpdir_path;
    else
	qDebug("Failed to create temporary directory: %s", qPrintable(tmpdir_path));
    return QString();
}

void Browser::start()
{
    if( started ) return;
    started = true;

    MsgBox::initButtonMap();

    if(!connection)
	connection = new Connection(this);
    else
	connection->disconnect();

    QString socketPath;
    QStringList args = QCoreApplication::arguments();

    if( args.size() > 1 )
	socketPath = createTmpDir(args.at(1));
    else
	socketPath = createTmpDir(socketPath);
    if( socketPath.isEmpty() )
	quitAppError(QStringLiteral("Failed to create local socket directory"));
    socketPath.append(QStringLiteral("/browser-sock"));

    qDebug("Socket path: %s",qPrintable(socketPath));
    mailbox = new MailBox(socketPath, this);

    connect(connection, SIGNAL(alteratorRequests()),
	    this, SLOT(onAlteratorRequests()));

    connect(connection, SIGNAL(startLongRequest()),
	    this, SLOT(onStartBusySplash()));
    connect(connection, SIGNAL(stopLongRequest()),
	    this, SLOT(onCheckBusySplash()));

    connection->init();
}

void Browser::stop()
{
    if( have_wm )
    {
	QRect geom = geometry();

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, QStringLiteral("alterator"), QStringLiteral("browser"), this);
	settings.setFallbacksEnabled(false);
	settings.beginGroup(QStringLiteral("qt"));
	settings.setValue(QStringLiteral("main_window_x"), geom.x());
	settings.setValue(QStringLiteral("main_window_y"), geom.y());
	settings.setValue(QStringLiteral("main_window_width"), geom.width());
	settings.setValue(QStringLiteral("main_window_height"), geom.height());
	settings.endGroup();
    }
}

bool Browser::quitApp(int answ)
{
    bool want_quit = false;
    switch( answ )
    {
	case QDialogButtonBox::Ok:
	case QDialogButtonBox::Yes:
	case QDialogButtonBox::YesToAll:
	{
	    want_quit = true;
	    quitAppManaged(0);
	    break;
	}
	default:
	{
	    want_quit = false;
	    break;
	}
    }
    return want_quit;
}

void Browser::quitAppManaged(int res)
{
    if( !in_quit ) {
	g_widgetlist->destroyAll();
	in_quit = true;
	QCoreApplication::exit(res);
    }
}

bool Browser::quitAppAsk()
{
    bool want_quit = false;
    MessageBox *msgbox = new MessageBox(QStringLiteral("warning"), tr("Quit"), tr("Exit Alterator?"), QDialogButtonBox::Ok|QDialogButtonBox::Cancel, this);
    want_quit = quitApp(msgbox->exec());
    popupRemove(msgbox);
    return want_quit;
}

void Browser::quitAppError(const QString &msg)
{
    MessageBox *msgbox = new MessageBox(QStringLiteral("critical"), tr("Quit"), msg, QDialogButtonBox::Ok, this);
    msgbox->exec();
    popupRemove(msgbox);
    quitAppManaged(1);
}

void Browser::closeEvent(QCloseEvent *e)
{
    if( g_browser->quitAppAsk() ) {
	e->accept();
    } else {
	e->ignore();
    }
}

void Browser::about()
{
    MessageBox *msgbox = new MessageBox(QStringLiteral("information"), tr("About"), tr("Alterator Browser %1").arg(QStringLiteral(PROGRAM_VERSION)), QDialogButtonBox::Ok, this);
    msgbox->exec();
    popupRemove(msgbox);
}

bool Browser::haveWindowManager()
{
    if( detect_wm_done ) return have_wm;

    have_wm = false;
    QString platform_name(qApp->platformName());
    qDebug("QPA platform: %s.", qPrintable(platform_name));
    if( platform_name == QStringLiteral("xcb")
	|| platform_name.contains(QStringLiteral("-xcomposite-")) ) {
	qDebug("X11 QPA platform");
#ifdef HAVE_X11
	Display *xdisplay = QX11Info::display();
	int screen_id = QX11Info::appScreen();

	Window xroot = RootWindow(xdisplay, screen_id);
	x_redirect_error = false;
	XSetErrorHandler(x_catchRedirectError);
	XSelectInput(xdisplay, xroot, SubstructureRedirectMask );
	XSync(xdisplay, false);
	if( x_redirect_error )
	{
	    have_wm = true;
	    //qDebug("X11 Window Manager detected");
	}
	else
	    qDebug("No X11 Window Manager detected");
#endif
    } else if( platform_name == QStringLiteral("wayland")
		|| (platform_name.startsWith(QStringLiteral("wayland"))
		    && !platform_name.contains(QStringLiteral("-xcomposite-"))) ) {
	qDebug("Wayland QPA platform");
    } else {
    }
    detect_wm_done = true;
    return have_wm;
}

void Browser::setFullScreen(bool full)
{
    if( full )
    {
	geometry_ = geometry();
	setGeometry(QGuiApplication::primaryScreen()->geometry());
    }
    else
	resize(geometry_.width(), geometry_.height());
}

void Browser::setHelpSource(const QString& str)
{
    help_browser->setHelpSource(str);
    QList<ACenterFace*> cfaces = g_widgetlist->aWidgetsByType<ACenterFace*>(WCenterFace);
    QListIterator<ACenterFace*> it(cfaces);
    while(it.hasNext())
	it.next()->setHelpSource(str);
}

void Browser::setHelpAvailable(bool avail)
{
    if(help_available && !avail)
        help_available = false;
}

void Browser::showHelp()
{
    if( help_available )
        QTimer::singleShot(0, help_browser, SLOT(exec()));
}

void Browser::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
	case Qt::Key_F1:
	{
	    if( help_available )
		QTimer::singleShot(0, help_browser, SLOT(exec()));
	    break;
	}
	case Qt::Key_Print:
	{
	    takeScreenShot();
	    break;
	}
	default:
	    BrowserBase::keyPressEvent(e);
    }
}

void Browser::changeLanguage(const QString& language)
{
    QString locale = language;
    if( locale.contains(QLatin1Char('_'))
	&& !(locale.contains(QLatin1Char('.')) || locale.contains(QLatin1Char('@'))) )
    {
	// && (locale != "C" && locale != "POSIX")
	locale += QStringLiteral(".UTF-8");
    }

    ::setenv("LC_ALL", locale.toLatin1(), true);
    ::setlocale(LC_ALL, locale.toLatin1());
    QLocale::setDefault( QLocale(locale) );

    reloadTranslator(&m_qt_translator, QStringLiteral("qtbase"), language);
    reloadTranslator(&m_app_translator, QStringLiteral("alterator_browser_qt"), language);

    Q_EMIT languageChanged();
}

void Browser::reloadTranslator(QTranslator** translator, const QString &domain,  const QString &language)
{
    if( *translator )
    {
	QCoreApplication::removeTranslator(*translator);
	delete *translator;
    }
    *translator = new QTranslator(this);
    (*translator)->setObjectName(domain);
    if( (*translator)->load(QLibraryInfo::location(QLibraryInfo::TranslationsPath) + QStringLiteral("/") + domain + QStringLiteral("_") + language) )
    {
	QCoreApplication::installTranslator(*translator);
    }
    else
    {
	delete *translator;
	*translator = 0;
    }
}

void Browser::emitEvent(const QString &id, const BrowserEventType type, const AlteratorRequestFlags request_flags)
{
	if( emit_locker > 0 ) return;
	if( request_flags & AlteratorRequestBlocking )
	{
//	    setEnabled(false);
	    ++emit_locker;
	}

	QString request = QStringLiteral("(alterator-request action \"event\"");
	request.append(QStringLiteral("name \""));
	request.append(QLatin1String(g_enums->browserEventToStr(type))); //append type
	request.append(QStringLiteral("\""));
	request.append(QStringLiteral("widget-id \""));
	request.append(id); //append id
	request.append(QStringLiteral("\""));
	
	//now collect a post data
	request += QStringLiteral("\n state (");
	request += g_widgetlist->makeRequestData();
	request += QStringLiteral("))"); //close message

	connection->getDocument(request, request_flags);

	if( request_flags & AlteratorRequestTimeReset )
	{
	    QList<ATimeEdit*> tm_edits = g_widgetlist->aWidgetsByType<ATimeEdit*>(WTimeEdit);
	    QListIterator<ATimeEdit*> it(tm_edits);
	    while(it.hasNext())
		it.next()->reset();
	}
}

void Browser::emitEvent(const QString &id,const QByteArray &type, const AlteratorRequestFlags request_flags)
{
    emitEvent(id, g_enums->strToBrowserEvent(type), request_flags);
}

void Browser::onAlteratorRequests()
{
    QQueue<AlteratorRequest> alterator_requests(connection->getRequests());
    while( !alterator_requests.isEmpty() )
	onAlteratorRequest(alterator_requests.dequeue());
}

void Browser::onAlteratorRequest(const AlteratorRequest& request)
{
    QList<QString> tab_order_parents;
    QMap<QString, QMap<int,QWidget*> > tab_order_list;

    if( request.flags & AlteratorRequestBlocking )
    {
	    --emit_locker;
//	    setEnabled(true);
    }
    if( request.flags & AlteratorRequestInit )
    {
	if( startup_splash )
	{
	    delete startup_splash;
	    startup_splash = 0;
	}
	if( !haveWindowManager() )
	{
	    QPalette pal = palette();
	    pal.setColor(QPalette::Window, m_color_window_default);
	    setPalette(pal);
	}
    }

    for(AlteratorRequestActionList::const_iterator it = (request.actions).begin(); it != (request.actions).end(); it++)
    {
	AlteratorRequestAction request = *it;
	switch( request.action )
	{
	    case AlteratorRequestNew:
	    {
		alWidget *new_wdg = onNewRequest(request.attr);
		if( request.attr.contains(QStringLiteral("tab-index")) )
		{
		    int tab_index = request.attr.take(QStringLiteral("tab-index")).i;
		    collectTabIndex(tab_order_parents, tab_order_list, new_wdg,
			tab_index);
		}
		request.attr.remove(QStringLiteral("widget-id"));
		request.attr.remove(QStringLiteral("type"));
		request.attr.remove(QStringLiteral("parent"));
		request.attr.remove(QStringLiteral("orientation"));
		request.attr.remove(QStringLiteral("columns"));
		request.attr.remove(QStringLiteral("colspan"));
		request.attr.remove(QStringLiteral("rowspan"));
		request.attr.remove(QStringLiteral("sub-type"));
		request.attr.remove(QStringLiteral("checked"));
		AlteratorRequestActionAttrs::iterator it;
		for(it = request.attr.begin(); it != request.attr.end(); it++ )
		{
		    onSetRequest(new_wdg->getId(), it.key(), it.value().s);
		}
		break;
	    }
	    case AlteratorRequestSet:
	    {
		QString attr_name = request.attr.value(QStringLiteral("attr-name")).s;
		if( attr_name != QStringLiteral("tab-index") )
		    onSetRequest(request.attr.value(QStringLiteral("widget-id")).s,
			attr_name,
			request.attr.value(QStringLiteral("attr-value")).s);
		else
		    collectTabIndex(tab_order_parents, tab_order_list,
			g_widgetlist->alWidgetById(request.attr.value(QStringLiteral("widget-id")).s),
			request.attr.value(QStringLiteral("attr-value")).s.toInt());
		break;
	    }
	    case AlteratorRequestClose:
	    {
		onCloseRequest(request.attr.value(QStringLiteral("widget-id")).s);
		break;
	    }
	    case AlteratorRequestClean:
	    {
		onCleanRequest(request.attr.value(QStringLiteral("widget-id")).s);
		break;
	    }
	    case AlteratorRequestEvent:
	    {
		onEventRequest(request.attr.value(QStringLiteral("widget-id")).s, request.attr.value(QStringLiteral("event-value")).s);
		break;
	    }
	    case AlteratorRequestSplash:
	    {
		onSplashMessageRequest(request.attr.value(QStringLiteral("splash-message")).s);
		break;
	    }
	    case AlteratorRequestStart:
	    {
		onStartRequest(request.attr.value(QStringLiteral("widget-id")).s);
		break;
	    }
	    case AlteratorRequestStop:
	    {
		onStopRequest(request.attr.value(QStringLiteral("widget-id")).s);
		break;
	    }
	    case AlteratorRequestMessage:
	    {
		onMessageBoxRequest(request.attr.value(QStringLiteral("message-type")).s, request.attr.value(QStringLiteral("message-title")).s,
			request.attr.value(QStringLiteral("message")).s, request.attr.value(QStringLiteral("buttons")).buttons);
		break;
	    }
	    case AlteratorRequestFile:
	    {
		onFileSelectRequest(request.attr.value(QStringLiteral("file-title")).s, request.attr.value(QStringLiteral("file-dir")).s,
		    request.attr.value(QStringLiteral("file-type")).s, request.attr.value(QStringLiteral("file-mask")).s);
		break;
	    }
	    case AlteratorRequestLanguage:
	    {
		changeLanguage(request.attr.value(QStringLiteral("language")).s);
		break;
	    }
	    case AlteratorRequestRetry:
	    {
		onRetryRequest();
		break;
	    }
	    case AlteratorRequestUnknown:
	    {
		qDebug("Unknown AlteratorRequest.");
		break;
	    }
	}
    }
    QListIterator<QString> parent_it(tab_order_parents);
    while( parent_it.hasNext() )
    {
	QMap<int,QWidget*> order_map = tab_order_list[parent_it.next()];
	QMapIterator<int,QWidget*> it(order_map);
	if( it.hasNext() )
	{
	    QWidget *from = it.next().value();
	    if( from->focusProxy() )
		from = from->focusProxy();
	    while( it.hasNext() )
	    {
		QWidget *to = it.next().value();
		if( to->focusProxy() )
		    to = to->focusProxy();
		setTabOrder(from, to);
		from = to;
	    }
	}
    }

    if( request.flags & AlteratorRequestCenterFaceModuleSelected )
    {
	QList<QWidget*> cf_views = g_widgetlist->viewVidgetsByQWidgetType<ACenterFace*>(WCenterFace);
	Q_FOREACH(QWidget *w, cf_views )
	    w->show();
    }

}

alWidget* Browser::onNewRequest(const AlteratorRequestActionAttrs &attr)
{
	AlteratorWidgetType widget_type = WUnknown;
	QString id;
	QString parent_id;
	QString columns;
	if(  attr.contains(QStringLiteral("widget-id")) )
	    id = attr.value(QStringLiteral("widget-id")).s;
	if(  attr.contains(QStringLiteral("type")) )
	    widget_type = attr.value(QStringLiteral("type")).t;
	else
	    qWarning("Error! no attribue id");
	if( attr.contains(QStringLiteral("parent")) )
	    parent_id = attr.value(QStringLiteral("parent")).s;
	else
	    qWarning("Error! no attribue parent id");

    /*
	qDebug("%s: id<%s> type<%s> parent_id<%s> sub-type<%s> width<%d> height<%d>", __FUNCTION__,
	    qPrintable(id), qPrintable(g_enums->widgetToStr(attr.value("type").t)), qPrintable(parent_id),
	    qPrintable(attr.value("sub-type").s),
	    attr.value("width").i, attr.value("height").i );
	*/

    alWidget *new_widget = 0;
    switch( widget_type )
    {
	case WDialog:
	case WMainWidget:
	{
	    if(parent_id.isEmpty()) {
	        new_widget = new alMainWidget(attr, id, QString());
	        PopupAddType pop_type = PopupMainNormalScreen;
		if( !haveWindowManager() ) {
		    QScreen* primary_screen = QGuiApplication::primaryScreen();
		    if( primary_screen ) {
			QRect screen_geom  = primary_screen->geometry();
			int width = screen_geom.width();
			int height = screen_geom.height();
			if( width > 100 && height > 100 && width/height >= 2 ) {
			    new_widget->getWidget()->setMaximumWidth(height*2);
			    pop_type = PopupMainWideScreen;
			}
		    }
		}
		popupAdd(new_widget->getWidget(), pop_type);
	    } else {
	        new_widget = new alDialog(attr,id,parent_id);
	    }
	    break;
	}
	case WBox:
	case WVBox:
	case WHBox: {        new_widget = new alBox(attr,id,parent_id); break; }
	case WVGroupBox:
	case WHGroupBox:
	case WGroupBox: {    new_widget = new alGroupBox(attr,id,parent_id); break; }
	case WButton: {      new_widget = new alButton(attr,id,parent_id); break; }
	case WLinkButton: {  new_widget = new alLinkButton(attr,id,parent_id); break; }
	case WRadio: {       new_widget = new alRadio(attr,id,parent_id); break; }
	case WLabel: {       new_widget = new alLabel(attr,id,parent_id); break; }
	case WEdit: {        new_widget = new alEdit(attr,id,parent_id); break; }
	case WTextBox: {     new_widget = new alTextBox(attr,id,parent_id); break; }
	case WGridBox: {     new_widget = new alGridBox(attr,id,parent_id); break; }
	case WCheckBox: {    new_widget = new alCheckBox(attr,id,parent_id); break; }
	case WTree: {        new_widget = new alTree(attr,id,parent_id); break; }
	case WComboBox: {    new_widget = new alComboBox(attr,id,parent_id); break; }
	case WTabBox: {      new_widget = new alTabBox(attr,id,parent_id); break; }
	case WProgressBar: { new_widget = new alProgressBar(attr,id,parent_id); break; }
	case WSlider: {      new_widget = new alSlider(attr,id,parent_id); break; }
	case WSeparator: {   new_widget = new alSeparator(attr,id,parent_id); break; }
	case WSpacer: {      new_widget = new alSpacer(attr,id,parent_id); break; }
	case WSpinBox: {     new_widget = new alSpinBox(attr,id,parent_id); break; }
	case WDateEdit: {    new_widget = new alDateEdit(attr,id,parent_id); break; }
	case WTimeEdit: {    new_widget = new alTimeEdit(attr,id,parent_id); break; }
	case WListBox:
	case WRadioListBox:
	case WMultiListBox:
	case WCheckListBox: {new_widget = new alListBox(widget_type,attr,id,parent_id); break; }
	case WCheckTree: {   new_widget = new alCheckTree(widget_type,attr,id,parent_id); break; }
	case WSlideShow: {   new_widget = new alSlideShow(attr,id,parent_id); break; }
	case WSplitBox: {    new_widget = new alSplitBox(attr,id,parent_id); break; }
	case WCenterFace: {  new_widget = new alCenterFace(attr,id,parent_id); break; }
	case WWizardFace: {  new_widget = new alWizardFace(attr,id,parent_id); break; }
	case WColorSelect: { new_widget = new alColorSelect(attr,id,parent_id); break; }
	case WFileSelect: {  new_widget = new alFileSelect(attr,id,parent_id); break; }
	case WDisplaySize: { new_widget = new alDisplaySize(attr,id,parent_id); break; }
	case WProxy:
	case WUnknown:
	{
	    qDebug("Unknown widget <%s> requested. Make gridbox instead.", g_enums->widgetToStr(attr.value(QStringLiteral("type")).t).data());
	    new_widget = new alGridBox(attr,id,parent_id);
	}
	/* default:
	    break; */
    }

    return new_widget;
}

void Browser::onCloseRequest(const QString& id)
{
    g_widgetlist->destroyLater(id);
}

void Browser::onCleanRequest(const QString& id)
{
	alWidget *el = g_widgetlist->alWidgetById(id);

	if( !el ) return;

	g_widgetlist->deleteChildrenById(id);

	QLayout* layout = el->getViewLayout();
	if( layout )
	{
	    for (int i = 0; i < layout->count(); ++i)
	    {
		QLayoutItem *li=layout->takeAt(i);
		if( li )
		    delete li;
	    }
	}
	
}

void Browser::onSetRequest(const QString& id,const QString& attr,const QString& value)
{
    /*
    qDebug("%s: id<%s> attr<%s> value<%s>", __FUNCTION__, qPrintable(id), qPrintable(attr), qPrintable(value));
    */
    alWidget *aw = g_widgetlist->alWidgetById(id);
    if( aw )
    {
        ++emit_locker;
        aw->setAttr(attr,value);
        --emit_locker;
    }
}

void Browser::onStartRequest(const QString& id)
{
	alWidget *aw = g_widgetlist->alWidgetById(id);
	if( aw )
	    aw->popUp();
}

void Browser::onStopRequest(const QString& id)
{
	alWidget *aw = g_widgetlist->alWidgetById(id);
	if( aw )
	    aw->popDown();
}

void Browser::onEventRequest(const QString& id,const QString& value)
{
    alWidget *aw = g_widgetlist->alWidgetById(id);
    if( aw )
    {
	aw->registerEvent(value);
    }
}

void Browser::onMessageBoxRequest(const QString& type, const QString& title,  const QString& message, QDialogButtonBox::StandardButtons buttons)
{
    MessageBox *msgbox = new MessageBox(type, title, message, buttons, this);
    int answ = msgbox->exec();
    connection->getDocument(MsgBox::unconvertButton((QDialogButtonBox::StandardButton)answ));
    popupRemove(msgbox);
}

void Browser::onFileSelectRequest(const QString& title, const QString& dir, const QString& type, const QString& mask)
{
    FileSelect *file_select = new FileSelect(this, title, dir, mask);
    QFileDialog::Options options = {};
    QFileDialog::ViewMode view_mode = QFileDialog::Detail;
    QFileDialog::FileMode file_mode = QFileDialog::AnyFile;
    QFileDialog::AcceptMode accept_mode = QFileDialog::AcceptOpen;
    Q_FOREACH(QString option, type.split(QLatin1Char(';'), Qt::SkipEmptyParts))
    {
	if( option == QStringLiteral("show_dirs_only") )
	    { options = options | QFileDialog::ShowDirsOnly; }
	else if( option == QStringLiteral("dont_resolve_symlinks") )
	    { options = options | QFileDialog::DontResolveSymlinks; }
	else if( option == QStringLiteral("dont_confirm_overwrite") )
	    { options = options | QFileDialog::DontConfirmOverwrite; }
	else if( option == QStringLiteral("read_only") )
	    { options = options | QFileDialog::ReadOnly; }
	else if( option == QStringLiteral("hide_name_filter_details") )
	    { options = options | QFileDialog::HideNameFilterDetails; }
	else if( option == QStringLiteral("detail") )
	    { view_mode = QFileDialog::Detail; }
	else if( option == QStringLiteral("list") )
	    { view_mode = QFileDialog::List; }
	else if( option == QStringLiteral("any_file") )
	    { file_mode = QFileDialog::AnyFile; }
	else if( option == QStringLiteral("existing_file") )
	    { file_mode = QFileDialog::ExistingFile; }
	else if( option == QStringLiteral("directory") )
	    { file_mode = QFileDialog::Directory; }
	else if( option == QStringLiteral("existing_files") )
	    { file_mode = QFileDialog::ExistingFiles; }
	else if( option == QStringLiteral("accept_open") )
	    { accept_mode = QFileDialog::AcceptOpen; }
	else if( option == QStringLiteral("accept_save") )
	    { accept_mode = QFileDialog::AcceptSave; }
    }
    file_select->setOptions(title, options, view_mode, file_mode, accept_mode);
    file_select->exec();
    connection->getDocument(file_select->selectedFiles().join(QLatin1Char(';')));
    popupRemove(file_select);
}

void Browser::splashStart()
{
	if (splash) return;
	splash = new SplashScreen(this);
	popupAdd(splash);
}

void Browser::splashStop()
{
    if( !splash ) return;
    popupRemove(splash);
    splash = 0;
}

void Browser::onSplashMessageRequest(const QString& msg)
{
	if (msg.isEmpty())
	{
	    alterator_splash = false;
	    if( !internal_splash && splash )
		splashStop();
	}
	else
	{
	    alterator_splash = true;
	    splashStart();
	    splash->setText(msg);
	}
}

void Browser::onInternalSplashMessage(const QString& msg)
{
	if (msg.isEmpty())
	{
	    internal_splash = false;
	    if( !alterator_splash && splash )
		splashStop();
	}
	else
	{
	    internal_splash = true;
	    splashStart();
	    if( !alterator_splash )
		splash->setText(msg);
	}
}

void Browser::getDocument(const QString& request)
{
    connection->getDocument(request);
}

void Browser::onStartBusySplash()
{
    Q_FOREACH(QWidget *widget, QApplication::topLevelWidgets()) {
	widget->setCursor(Qt::WaitCursor);
    }
}

void Browser::onCheckBusySplash()
{
    busy_timer->start();
}

void Browser::onStopBusySplash()
{
    Q_FOREACH(QWidget *widget, QApplication::topLevelWidgets()) {
	widget->unsetCursor();
    }
}

void Browser::onRetryRequest()
{
    QTimer::singleShot(50, this,SLOT(doRetry()));
}

void Browser::doRetry()
{
    connection->getDocument(QStringLiteral("(alterator-request action \"re-get\")"));
}

void Browser::loadBranding()
{
    QResource::unregisterResource(QStringLiteral("/etc/alterator/design-browser-qt"));
    QResource::registerResource(QStringLiteral("/etc/alterator/design-browser-qt"));

    QFont font_def(QGuiApplication::font());
    {
	QScreen *primary_screen = QGuiApplication::primaryScreen();
	if( primary_screen && primary_screen->geometry().height() > 800 ) {
	    font_def.setPointSize(font_def.pointSize()+2); // increase default value
	}
    }

    if( QFile::exists(QStringLiteral(":/design/design.ini")) )
    {

	QSettings settings(QStringLiteral(":/design/design.ini"), QSettings::IniFormat, this);
	settings.setFallbacksEnabled(false);
	QStringList keys(settings.allKeys());
	QString key;

	// set Qt style
	key = QStringLiteral("style");
	if( !keys.contains(key) ) {
	    key = QStringLiteral("Style");
	}
	if( keys.contains(key) ) {
	    QString styleName = settings.value(key).toString();
	    if( QStyleFactory::keys().contains(styleName, Qt::CaseInsensitive) )
		qApp->setStyle(styleName);
	}

	// set icons theme
	key = QStringLiteral("icons");
	if( !keys.contains(key) ) {
	    key = QStringLiteral("Icons");
	}
	if( keys.contains(key) ) {
	    QString iconsTheme = settings.value(key).toString();
	    if( !iconsTheme.isEmpty() )
		QIcon::setThemeName(iconsTheme);
	}
	key = QStringLiteral("icons_fallback");
	if( !keys.contains(key) ) {
	    key = QStringLiteral("Icons_fallback");
	}
	if( !keys.contains(key) ) {
	    key = QStringLiteral("Icons_Fallback");
	}
	if( keys.contains(key) ) {
	    QString fallIconsTheme = settings.value(key).toString();
	    if( !fallIconsTheme.isEmpty() )
		QIcon::setFallbackThemeName(fallIconsTheme);
	}

	// set palette
	QStringList strlist;
	int i;
	QPalette pal(Qt::black);
	int groupCount = 0;
	key = QStringLiteral("Palette/active");
	if( !keys.contains(key) ) {
	    key = QStringLiteral("palette/active");
	}
	strlist = settings.value(key).toStringList();
	if (strlist.count() == QPalette::NColorRoles)
	{
	    ++groupCount;
	    for (i = 0; i < QPalette::NColorRoles; i++)
		pal.setColor(QPalette::Active, (QPalette::ColorRole) i, QColor(strlist[i]));
	}
	key = QStringLiteral("Palette/inactive");
	if( !keys.contains(key) ) {
	    key = QStringLiteral("palette/inactive");
	}
	strlist = settings.value(key).toStringList();
	if (strlist.count() == QPalette::NColorRoles)
	{
	    ++groupCount;
	    for (i = 0; i < QPalette::NColorRoles; i++)
		pal.setColor(QPalette::Inactive, (QPalette::ColorRole) i, QColor(strlist[i]));
	}
	key = QStringLiteral("Palette/disabled");
	if( !keys.contains(key) ) {
	    key = QStringLiteral("palette/disabled");
	}
	strlist = settings.value(key).toStringList();
	if(strlist.count() == QPalette::NColorRoles)
	{
	    ++groupCount;
	    for (i = 0; i < QPalette::NColorRoles; i++)
	    pal.setColor(QPalette::Disabled, (QPalette::ColorRole) i, QColor(strlist[i]));
	}
	if( groupCount == QPalette::NColorGroups )
	    QApplication::setPalette(pal);

	// get font
	key = QStringLiteral("font");
	if( !keys.contains(key) ) {
	    key = QStringLiteral("Font");
	}
	if( keys.contains(key) ) {
	    QString str = settings.value(key).toString();
	    if (!str.isEmpty()) {
		font_def.fromString(str);
	    }
	}
    }
    QGuiApplication::setFont(font_def);

    // set style
    QString new_style;
    QFile file(QStringLiteral(":/design/design.qss"));
    if( file.exists() )
    {
	if( file.open(QFile::ReadOnly) )
	{
	    QString styleContent = QLatin1String(file.readAll());
	    if( styleContent.size() < 10 )
	    {
		qDebug("Too small file: \"%s\"", qPrintable(file.fileName()));
		return;
	    }
	    new_style = styleContent;
	}
	else
	{
	    qDebug("Unable to read file: \"%s\"", qPrintable(file.fileName()));
	    return;
	}
    }
    qApp->setStyleSheet(new_style);
}

void Browser::collectTabIndex(QList<QString>& parents, QMap<QString, QMap<int,QWidget*> >& order, alWidget *wdg, int tab_index)
{
    if( tab_index >= 0 && wdg )
    {
        QString parent_id = wdg->getParentId();
        if( !parent_id.isEmpty() )
        {
	    if( !parents.contains(parent_id) )
		parents.append(parent_id);
	    order[parent_id].insert(tab_index, wdg->getWidget());
	}
    }
}

void Browser::popupAdd(QWidget *pop, PopupAddType popup_type)
{
    if( !pop ) return;

    QWidget *cw = central_layout->currentWidget();
    if( cw )
	cw->setEnabled(false);

    QWidget *widget_to_add;

    if( popup_type == PopupDialog )
    {
	widget_to_add = new QWidget(central_widget);
	QHBoxLayout *hl = new QHBoxLayout(widget_to_add);
	QVBoxLayout *vl = new QVBoxLayout();
	hl->addStretch(1);
	hl->addLayout(vl);
	hl->addStretch(1);
	vl->addStretch(1);
	vl->addWidget(pop);
	vl->addStretch(1);
    } else if( popup_type == PopupMainWideScreen ) {
	widget_to_add = new QWidget(central_widget);
	QHBoxLayout *hl = new QHBoxLayout(widget_to_add);
	hl->setContentsMargins(0,0,0,0);
	hl->addStretch(1);
	hl->addWidget(pop, 2);
	hl->addStretch(1);
    } else {
	widget_to_add = pop;
    }

    central_layout->addWidget(widget_to_add);
    central_layout->setCurrentWidget(widget_to_add);
    widget_to_add->setEnabled(true);
    popups[pop] = widget_to_add;

    if( popup_type == PopupDialog )
	pop->setFocus();
}

void Browser::popupRemove(QWidget *p)
{
    if( !popups.contains(p) )
    {
	qWarning("Failed to remove unexistent popup.");
	return;
    }
    QWidget *stacked = popups.take(p);
    central_layout->removeWidget(stacked);
    stacked->deleteLater();

    QWidget *cw = central_layout->currentWidget();
    if( cw )
	cw->setEnabled(true);
}

void Browser::raiseBrowserWindow()
{
	QWidget *wnd = window();
	wnd->setWindowState(wnd->windowState() & ~Qt::WindowMinimized);
	wnd->activateWindow();
	wnd->raise();
}

void Browser::onUnixSignal(int sig)
{
    unixSignalHandler(sig);
}

void Browser::unixSignalHandler(int sig)
{
    switch(sig)
    {
	case SIGUSR1:
	    {
		if( g_browser )
		    g_browser->raiseBrowserWindow();
		break;
	    }
	default:
	    break;
    }
}

QString Browser::shortLang()
{
    if( m_shortlang.isEmpty() )
    {
	m_shortlang = QLocale::system().name().remove(QRegularExpression(QStringLiteral("_.*")));
    }

    return m_shortlang;
}

void Browser::takeScreenShot(QWidget *wnd2) {
    QWidget *wnd(window());
    if( wnd ) {
	bool is_first_screenshot = m_first_screenshot;
	QPixmap pix = wnd->grab();
	QPixmap pix2;
	QPoint pos1(wnd->mapToGlobal(QPoint(0,0)));
	QPoint pos2;
	if( wnd2 ) {
	    pix2 = wnd2->grab();
	    pos2 = wnd2->mapToGlobal(QPoint(0,0));
	}
	if( !pix.isNull() ) {
	    if( !pix2.isNull() ) {
		QPainter paint(&pix);
		paint.drawPixmap(pos2.x()-pos1.x(), pos2.y()-pos1.y(), pix2);
	    }
	    QString tmp_path(qEnvironmentVariable("TMPDIR", QStringLiteral("/tmp")));
	    QString scrpath(QLatin1String("%1/alterator-screenshot-%2-%3.png").arg(tmp_path).arg(QDateTime::currentDateTime().toString(QStringLiteral("yyyy.MM.dd-HH.mm"))).arg(Utils::getRandomString(4)));
	    QString msg_type;
	    QString msg_text;
	    QDialogButtonBox::StandardButtons msg_buttons = QDialogButtonBox::NoButton;
	    if( is_first_screenshot ) {
		msg_text = tr("Images will be found at<br/><strong>/root/.install-log/</strong><br/>after install. Now saved to<br/><strong>%1</strong>").arg(tmp_path);
		msg_buttons = QDialogButtonBox::Ok;
	    } else {
		msg_text = tr("Path: %1").arg(scrpath);
		msg_buttons = QDialogButtonBox::NoButton;
	    }
	    if( pix.save(scrpath, nullptr) ) {
		msg_type = QLatin1String("information");
		m_first_screenshot = false;
		qWarning("Save screenshot to %s.", qPrintable(scrpath));
	    } else {
		msg_type = QLatin1String("critical");
	    }
	    MessageBox *msgbox = new MessageBox(msg_type, tr("Screenshot"), msg_text, msg_buttons, this);
	    if( is_first_screenshot ) {
		msgbox->exec();
		popupRemove(msgbox);
	    } else {
		msgbox->execForTimeout(2500);
	    }
	} else {
	    qWarning("Screenshot is null pixmap.");
	}
    }
}
