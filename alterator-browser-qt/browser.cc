//#include <sys/types.h>
#include <sys/wait.h>

#include <QTextStream>
#include <QDataStream>
#include <QTimer>
#include <QCursor>
#include <QSettings>
#include <QApplication>
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

#include "global.hh"
#include "widgets.hh"
#include "connection.hh"
#include "browser.hh"
#include "messagebox.hh"
#include "constraints.hh"
#include "mailbox.hh"
#include "widgetlist.hh"
#include "a_pixmaps.hh"
#include "enums.hh"
#include "fileselect.hh"

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <QX11Info>
#endif

MailBox *mailbox = 0;
WidgetList *widgetlist = 0;


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

Browser::Browser():
    Browser_t(0)
{
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    qRegisterMetaType<AlteratorRequest>("AlteratorRequest");
    loadStyleSheet();

    internal_splash = false;
    alterator_splash = false;
    splash = 0;
    emit_locker = 0; //wrong emit protector
    connection = 0;
    constraints = 0;
    qtranslator = 0;
    app_translator = 0;
    busy_timer_id = 0;
    started = false;
    in_quit = false;
    detect_wm = false;
    help_available = true;
    central_widget = 0;
    central_layout = 0;
    widgetlist = new WidgetList(this);
    pixmap_paths
	<< ":/design/"
	<< "/usr/share/alterator/design/images/"
	<< "/usr/share/alterator/images/";

    QString language(getenv("LC_ALL"));
    if( language.isEmpty() )
	language = getenv("LC_MESSAGES");
    if( language.isEmpty() )
	language = getenv("LANG");
    if( language.isEmpty() )
	language = "C";
    changeLanguage(language);

    window()->setWindowTitle("alterator");

    central_widget = new QWidget(this);
    central_layout = new QStackedLayout(central_widget);
    central_layout->setStackingMode(QStackedLayout::StackAll);
    setCentralWidget(central_widget);

    // startup animation
    startup_splash = new QLabel(this);
    startup_splash->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    startup_splash->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    startup_splash->setAlignment( Qt::AlignCenter );
    central_layout->addWidget(startup_splash);

    QString startup_splash_file = ":/design/whirl.mng";
    if( !QFile::exists(startup_splash_file) )
	startup_splash_file = ":/images/whirl.mng";
    QMovie *anim = new QMovie(startup_splash_file);
    if( anim->isValid() )
    {
	startup_splash->setMovie(anim);
	anim->start();
    }

    help_browser = new HelpBrowser(this);
    have_wm = haveWindowManager();
    if( have_wm )
    {
	QRect desktop_geom  = QApplication::desktop()->geometry();
	int wnd_recom_width = (int)(desktop_geom.width()/1.5);
	int wnd_recom_height = (int)(desktop_geom.height()/1.5);

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "alterator", "browser", this);
	settings.setFallbacksEnabled(false);
	settings.beginGroup("qt");
	int wnd_x = settings.value("main_window_x", 0).toInt();
	int wnd_y = settings.value("main_window_y", 0).toInt();
	int wnd_width = settings.value("main_window_width", wnd_recom_width).toInt();
	int wnd_height = settings.value("main_window_height", wnd_recom_height).toInt();
	settings.endGroup();

	geometry_ = QRect(wnd_x, wnd_y, wnd_width, wnd_height);
	setGeometry(geometry_);
    }
    else
	setFullScreen(true);

    connect(QApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(stop()));
    QTimer::singleShot(0, this, SLOT(start()));
}

Browser::~Browser()
{
}

void Browser::start()
{
    if( started ) return;
    started = true;

    MsgBox::initButtonMap();

    if(!constraints)
	constraints = new Constraints(this);
    else
	constraints->disconnect();

    if(!connection)
	connection = new Connection(this);
    else
	connection->disconnect();

    QString socketPath;
    QStringList args = QCoreApplication::arguments();
    QString tmpdir(getenv("TMPDIR"));

    if( args.size() > 1 )
    	socketPath = args.at(1);
    else if( !tmpdir.isEmpty() )
    	socketPath = tmpdir;
    else
    	socketPath = "/tmp";

    QDir socketDir(socketPath);
    if(!socketDir.exists("alterator"))
    {
	if(!socketDir.mkdir("alterator"))
	{
	    quitAppError("Failed to create local socket directory");
	}
    }

    socketPath += "/alterator/browser-sock";
    qDebug("socket path %s ...",qPrintable(socketPath));
    mailbox = new MailBox(socketPath, this);

    connect(connection, SIGNAL(alteratorRequest(const AlteratorRequest&)),
	    this, SLOT(onAlteratorRequest(const AlteratorRequest&)));

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

	QSettings settings(QSettings::IniFormat, QSettings::UserScope, "alterator", "browser", this);
	settings.setFallbacksEnabled(false);
	settings.beginGroup("qt");
	settings.setValue("main_window_x", geom.x());
	settings.setValue("main_window_y", geom.y());
	settings.setValue("main_window_width", geom.width());
	settings.setValue("main_window_height", geom.height());
	settings.endGroup();
    }
}

void Browser::quitApp(int answ)
{
    switch( answ )
    {
	case QDialogButtonBox::Ok:
	case QDialogButtonBox::Yes:
	case QDialogButtonBox::YesToAll:
	{
	    quitAppManaged(0);
	    break;
	}
	default:
	{
	    popupRemoveCurrent(answ);
	    break;
	}
    }
}

void Browser::quitAppManaged(int res)
{
    widgetlist->destroyAll();
    in_quit = true;
    QCoreApplication::exit(res);
}

void Browser::quitAppAsk()
{
    MessageBox *msgbox = new MessageBox("warning", tr("Quit"), tr("Exit Alterator?"), QDialogButtonBox::Ok|QDialogButtonBox::Cancel, this);
    quitApp(msgbox->exec());
}

void Browser::quitAppError(const QString &msg)
{
    MessageBox *msgbox = new MessageBox("critical", tr("Quit"), msg, QDialogButtonBox::Ok|QDialogButtonBox::Cancel, this);
    msgbox->exec();
    quitAppManaged(1);
}

void Browser::closeEvent(QCloseEvent *e)
{
    if( !in_quit )
	quitAppManaged(0);
    e->ignore();
}

void Browser::about()
{
    MessageBox *msgbox = new MessageBox("information", tr("About"), tr("Alterator Browser"), QDialogButtonBox::Ok, this);
    msgbox->exec();
    popupRemoveCurrent(QDialogButtonBox::Close);
}

bool Browser::haveWindowManager()
{
    if( detect_wm )
	return have_wm;

	have_wm = false;
#ifdef Q_WS_X11
	const QX11Info xinfo = x11Info();
	Display *xdisplay = xinfo.display();
	int screen_id = xinfo.appScreen();

	Window xroot = RootWindow(xdisplay, screen_id);
	x_redirect_error = false;
	XSetErrorHandler(x_catchRedirectError);
	XSelectInput(xdisplay, xroot, SubstructureRedirectMask );
	QApplication::syncX();
	if( x_redirect_error )
	{
	    have_wm = true;
	    //qDebug("Window Manager detected");
	}
	else
	    qDebug("No Window Manager detected");
#endif
    detect_wm = true;
    return have_wm;
}

void Browser::setFullScreen(bool full)
{
    if( full )
    {
	geometry_ = geometry();
	setGeometry(QApplication::desktop()->geometry());
    }
    else
	setGeometry(geometry_);
}

void Browser::setHelpSource(const QString& str)
{
    help_browser->setHelpSource(str);
    QList<ACenterFace*> cfaces = widgetlist->aWidgetsByType<ACenterFace*>(WCenterFace);
    QListIterator<ACenterFace*> it(cfaces);
    while(it.hasNext())
	it.next()->setHelpSource(str);
}

void Browser::setHelpAvailable(bool avail)
{
    if(help_available && !avail)
        help_available = false;
}

bool Browser::event(QEvent* e)
{
    if( e->type() == EVENT_HELP )
    {
	if( help_available )
	    QTimer::singleShot(0, help_browser, SLOT(exec()));
    }
    return QMainWindow::event(e);
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
	default:
	    break;
    }
#if 0
    if(e->key() == Qt::Key_Return)
    {
#if 1
	qDebug("%s: Key_Return released", __FUNCTION__);
	if( current_step && current_step->info()->isNextEnabled() )
	    QTimer::singleShot(0, current_step, SLOT(next()));
#else
	QObjectList *list = queryList( "QPushButton" );
	QObjectListIt it( *list );
	QPushButton *pb;
	while ( (pb = (QPushButton*)it.current()) )
	{
	    if ( pb->isDefault() && pb->isVisible() )
	    {
	        delete list;
		if ( pb->isEnabled() )
		{
		    emit pb->clicked();
		}
	    return;
	    }
	    ++it;
	}
	delete list;
#endif
    }
#endif
}

void Browser::changeLanguage(const QString& language)
{
    QString locale = language;
    if( locale.contains("_")
	&& !(locale.contains(".") || locale.contains("@")))
    {
	// && (locale != "C" && locale != "POSIX")
	locale += ".UTF-8";
    }

    ::setenv("LC_ALL", locale.toLatin1(), true);
    ::setlocale(LC_ALL, locale.toLatin1());
    new QSystemLocale();
    QLocale::setDefault( QLocale(locale) );

    reloadTranslator(qtranslator, "qt");
    reloadTranslator(app_translator, "alterator_browser_qt");

    emit languageChanged();
}

void Browser::reloadTranslator(QTranslator* translator, const QString &domain)
{
    if( translator )
    {
	QCoreApplication::removeTranslator(translator);
	delete translator;
    }
    translator = new QTranslator(this);
    if( translator->load(QLibraryInfo::location(QLibraryInfo::TranslationsPath) + "/" + domain + "_"+QLocale::system().name()) )
    {
	QCoreApplication::installTranslator(translator);
    }
    else
    {
	delete translator;
	translator = 0;
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

	QString request = "(alterator-request action \"event\"";
	request += "name \""+enums->browserEventToStr(type)+"\"";//append type
	request += "widget-id \""+id+"\"";//append id
	
	//now collect a post data
	request += "\n state (";
	request += widgetlist->makeRequestData();	
	request += "))"; //close message

	connection->getDocument(request, request_flags);

	if( request_flags & AlteratorRequestTimeReset )
	{
	    QList<ATimeEdit*> tm_edits = widgetlist->aWidgetsByType<ATimeEdit*>(WTimeEdit);
	    QListIterator<ATimeEdit*> it(tm_edits);
	    while(it.hasNext())
		it.next()->reset();
	}
}

void Browser::emitEvent(const QString &id,const QString &type, const AlteratorRequestFlags request_flags)
{
    emitEvent(id, enums->strToBrowserEvent(type), request_flags);
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
    }

    for(AlteratorRequestActionList::const_iterator it = (request.actions).begin(); it != (request.actions).end(); it++)
    {
	AlteratorRequestAction request = *it;
	switch( request.action )
	{
	    case AlteratorRequestNew:
	    {
		alWidget *new_wdg = onNewRequest(request.attr);
		collectTabIndex(tab_order_parents, tab_order_list, new_wdg,
		    request.attr[AltReqParamWTabIndex].i);
		break;
	    }
	    case AlteratorRequestSet:
	    {
		QString attr_name = request.attr[AltReqParamWAttrName].s;
		if( attr_name != "tab-index" )
		    onSetRequest(request.attr[AltReqParamWId].s,
			attr_name,
			    request.attr[AltReqParamWAttrValue].s);
		else
		    collectTabIndex(tab_order_parents, tab_order_list,
			widgetlist->alWidgetById(request.attr[AltReqParamWId].s),
			    request.attr[AltReqParamWTabIndex].i);
		break;
	    }
	    case AlteratorRequestClose:
	    {
		onCloseRequest(request.attr[AltReqParamWId].s);
		break;
	    }
	    case AlteratorRequestClean:
	    {
		onCleanRequest(request.attr[AltReqParamWId].s);
		break;
	    }
	    case AlteratorRequestEvent:
	    {
		onEventRequest(request.attr[AltReqParamWId].s, request.attr[AltReqParamEventValue].s);
		break;
	    }
	    case AlteratorRequestSplash:
	    {
		onSplashMessageRequest(request.attr[AltReqParamSplashMessage].s);
		break;
	    }
	    case AlteratorRequestStart:
	    {
		onStartRequest(request.attr[AltReqParamWId].s);
		break;
	    }
	    case AlteratorRequestStop:
	    {
		onStopRequest(request.attr[AltReqParamWId].s);
		break;
	    }
	    case AlteratorRequestMessage:
	    {
		onMessageBoxRequest(request.attr[AltReqParamMessageType].s, request.attr[AltReqParamMessageTitle].s,
			request.attr[AltReqParamMessage].s, request.attr[AltReqParamButtons].buttons);
		break;
	    }
	    case AlteratorRequestFile:
	    {
		onFileSelectRequest(request.attr[AltReqParamFileTitle].s, request.attr[AltReqParamFileDir].s,
		    request.attr[AltReqParamFileType].s, request.attr[AltReqParamFileMask].s);
		break;
	    }
	    case AlteratorRequestLanguage:
	    {
		changeLanguage(request.attr[AltReqParamLanguage].s);
		break;
	    }
	    case AlteratorRequestRetry:
	    {
		onRetryRequest();
		break;
	    }
	    case AlteratorRequestCnstrAdd:
	    {
		constraints->add(request.attr[AltReqParamCnstrName].s,
		    request.attr[AltReqParamCnstrType].s,
		    request.attr[AltReqParamCnstrParams].s);
		break;
	    }
	    case AlteratorRequestCnstrClear:
	    {
		constraints->clear();
		break;
	    }
	    case AlteratorRequestCnstrApply:
	    {
		constraints->apply();
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
	    while( it.hasNext() )
	    {
		QWidget *to = it.next().value();
		setTabOrder(from, to);
		from = to;
	    }
	}
    }

    if( request.flags & AlteratorRequestCenterFaceModuleSelected )
    {
	QList<QWidget*> cf_views = widgetlist->viewVidgetsByQWidgetType<ACenterFace*>(WCenterFace);
	foreach(QWidget *w, cf_views )
	    w->show();
    }

}

alWidget* Browser::onNewRequest(const AlteratorRequestActionAttrs &attr)
{
	QString id = attr[AltReqParamWId].s;
	QString parent_id = attr[AltReqParamWParentId].s;
	Qt::Orientation orientation = attr[AltReqParamWOrientation].o;
	QString columns = attr[AltReqParamWColumns].s;
	alWidget *new_widget = 0;

    /*
	qDebug("%s: id<%s> type<%s> parent_id<%s> orientation<%s> sub-type<%s> width<%d> height<%d> columns<%s>", __FUNCTION__,
	    qPrintable(id), qPrintable(enums->widgetToStr(attr[AltReqParamWType].t)), qPrintable(parent_id),
	    orientation == Qt::Horizontal ? "-":"|", qPrintable(attr[AltReqParamWSubType].s),
	    attr[AltReqParamWWidth].i, attr[AltReqParamWHeight].i, qPrintable(columns) );
	*/

    switch( attr[AltReqParamWType].t )
    {
	case WDialog:
	case WMainWidget:
	{
	    if(parent_id.isEmpty())
	        new_widget = new alMainWidget(attr, id, "");
	    else
	        new_widget = new alDialog(attr,id,parent_id);
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
	case WGridBox: {     new_widget = new alGridBox(attr,id,parent_id, columns); break; }
	case WCheckBox: {    new_widget = new alCheckBox(attr,id,parent_id); break; }
	case WTree: {        new_widget = new alTree(attr,id,parent_id, columns); break; }
	case WComboBox: {    new_widget = new alComboBox(attr,id,parent_id); break; }
	case WHTabBox:
	case WVTabBox:
	case WTabBox: {      new_widget = new alTabBox(attr,id,parent_id,orientation); break; }
	case WVTabPage:
	case WHTabPage:
	case WTabPage: {     new_widget = new alTabPage(attr,id,parent_id,orientation); break; }
	case WProgressBar: { new_widget = new alProgressBar(attr,id,parent_id); break; }
	case WSlider: {      new_widget = new alSlider(attr,id,parent_id); break; }
	case WSeparator: {   new_widget = new alSeparator(attr,id,parent_id,orientation); break; }
	case WSpacer: {      new_widget = new alSpacer(attr,id,parent_id); break; }
	case WSpinBox: {     new_widget = new alSpinBox(attr,id,parent_id); break; }
	case WDateEdit: {    new_widget = new alDateEdit(attr,id,parent_id); break; }
	case WTimeEdit: {    new_widget = new alTimeEdit(attr,id,parent_id); break; }
	case WListBox:
	case WRadioListBox:
	case WMultiListBox:
	case WCheckListBox: {new_widget = new alListBox(attr[AltReqParamWType].t,attr,id,parent_id,columns.toInt()); break; }
	case WSlideShow: {   new_widget = new alSlideShow(attr,id,parent_id); break; }
	case WSplitBox: {    new_widget = new alSplitBox(attr,id,parent_id,orientation,columns); break; }
	case WCenterFace: {  new_widget = new alCenterFace(attr,id,parent_id); break; }
	case WWizardFace: {  new_widget = new alWizardFace(attr,id,parent_id,orientation); break; }
	case WProxy:
	case WUnknown:
	{
	    qDebug("Unknown widget <%s> requested. Make gridbox instead.", qPrintable(enums->widgetToStr(attr[AltReqParamWType].t)));
	    new_widget = new alGridBox(attr,id,parent_id, "100");
	}
	/* default:
	    break; */
    }

	//
	if( new_widget )
	{
	    int width = attr[AltReqParamWWidth].i;
	    if( width  > 0 )
		new_widget->setAttr("width",  QString("%1").arg(width));
	    int height = attr[AltReqParamWHeight].i;
	    if( height > 0 )
		new_widget->setAttr("height", QString("%1").arg(height));
	}
    return new_widget;
}

void Browser::onCloseRequest(const QString& id)
{
    widgetlist->destroyLater(id);
}

void Browser::onCleanRequest(const QString& id)
{
	alWidget *el = widgetlist->alWidgetById(id);

	if( !el ) return;

	widgetlist->deleteChildrenById(id);

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
    alWidget *aw = widgetlist->alWidgetById(id);
    if( aw )
    {
        ++emit_locker;
        aw->setAttr(attr,value);
        --emit_locker;
    }
}

void Browser::onStartRequest(const QString& id)
{
	alWidget *aw = widgetlist->alWidgetById(id);
	if( aw )
	    aw->popUp();
}

void Browser::onStopRequest(const QString& id)
{
	alWidget *aw = widgetlist->alWidgetById(id);
	if( aw )
	    aw->popDown();
}

void Browser::onEventRequest(const QString& id,const QString& value)
{
    alWidget *aw = widgetlist->alWidgetById(id);
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
    popupRemoveCurrent(answ);
}

void Browser::onFileSelectRequest(const QString& title, const QString& dir, const QString& type, const QString& mask)
{
    FileSelect *file_select = new FileSelect(this, title, dir);
    file_select->exec();
    connection->getDocument(file_select->selectedFiles().join(";"));
    popupRemoveCurrent(QDialogButtonBox::Close);
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
    splash = 0;
    popupRemoveCurrent(QDialogButtonBox::Close);
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
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
	widget->setCursor(Qt::WaitCursor);
    }
}

void Browser::onCheckBusySplash()
{
    if( busy_timer_id > 0 )
	killTimer(busy_timer_id);
    busy_timer_id = startTimer(500);
}

void Browser::onStopBusySplash()
{
    killTimer(busy_timer_id);
    busy_timer_id = 0;
    foreach (QWidget *widget, QApplication::topLevelWidgets()) {
	widget->unsetCursor();
    }
}

void Browser::timerEvent(QTimerEvent *e)
{
    if( e->timerId() == busy_timer_id )
	onStopBusySplash();
}

void Browser::onRetryRequest()
{
    QTimer::singleShot(50, this,SLOT(doRetry()));
}

void Browser::doRetry()
{
    connection->getDocument("(alterator-request action \"re-get\")");
}

void Browser::loadStyleSheet()
{
    QResource::unregisterResource("/etc/alterator/design-browser-qt");
    QResource::registerResource("/etc/alterator/design-browser-qt");

    if( QFile::exists(":/design/design.ini") )
    {
	QSettings settings(":/design/design.ini", QSettings::IniFormat, this);
	settings.setFallbacksEnabled(false);

	// set Qt style
	QString styleName = settings.value("style", "Plastique").toString();
	if( !QStyleFactory::keys().contains(styleName) )
	    styleName = "Plastique";
	qApp->setStyle(styleName);


	// set palette
	QStringList strlist;
	int i;
	QPalette pal(Qt::black);
	int groupCount = 0;
	strlist = settings.value(QLatin1String("Palette/active")).toStringList();
	if (strlist.count() == QPalette::NColorRoles)
	{
	    ++groupCount;
	    for (i = 0; i < QPalette::NColorRoles; i++)
		pal.setColor(QPalette::Active, (QPalette::ColorRole) i, QColor(strlist[i]));
	}
	strlist = settings.value(QLatin1String("Palette/inactive")).toStringList();
	if (strlist.count() == QPalette::NColorRoles)
	{
	    ++groupCount;
	    for (i = 0; i < QPalette::NColorRoles; i++)
		pal.setColor(QPalette::Inactive, (QPalette::ColorRole) i, QColor(strlist[i]));
	}
	strlist = settings.value(QLatin1String("Palette/disabled")).toStringList();
	if(strlist.count() == QPalette::NColorRoles)
	{
	    ++groupCount;
	    for (i = 0; i < QPalette::NColorRoles; i++)
	    pal.setColor(QPalette::Disabled, (QPalette::ColorRole) i, QColor(strlist[i]));
	}
	if( groupCount == QPalette::NColorGroups )
	    QApplication::setPalette(pal);

	// set font
        QFont font(QApplication::font());
        QString str = settings.value(QLatin1String("font")).toString();
        if (!str.isEmpty())
	{
            font.fromString(str);
            if (font != QApplication::font())
                QApplication::setFont(font);
        }
    }

    // set style
    QFile file(":/design/design.qss");
    if( file.exists() )
    {
	if( file.open(QFile::ReadOnly) )
	{
	    QString styleContent = file.readAll();
	    if( styleContent.size() < 10 )
	    {
		qDebug("Too small file: \"%s\"", qPrintable(file.fileName()));
		return;
	    }
	    qApp->setStyleSheet(styleContent);
	}
	else
	{
	    qDebug("Unable to read file: \"%s\"", qPrintable(file.fileName()));
	    return;
	}
    }
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

void Browser::popupAdd(QWidget *pop, bool simple)
{
    if( !pop ) return;

    QWidget *cw = central_layout->currentWidget();
    if( cw )
	cw->setEnabled(false);

    QWidget *widget_to_add = pop;

    if( !simple )
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
    }

    central_layout->addWidget(widget_to_add);
    central_layout->setCurrentWidget(widget_to_add);
    widget_to_add->setEnabled(true);

    if( !simple )
	widget_to_add->setFocus();
}

void Browser::popupRemoveCurrent(int)
{
    int ci = central_layout->currentIndex();
    QWidget *cw = central_layout->currentWidget();
    if( cw )
	cw->deleteLater();
    if( --ci >= 0 )
    {
	QWidget *cw = central_layout->widget(ci);
	if( cw )
	    cw->setEnabled(true);
    }
}

void Browser::onUnixSignal(int sig)
{
    switch(sig)
    {
	case SIGUSR1:
	    {
		QWidget *wnd = window();
		if( wnd->isMaximized() )
		    wnd->showMaximized();
		else
		    wnd->showNormal();
		wnd->show();
		wnd->activateWindow();
		wnd->raise();
		break;
	    }
	default:
	    break;
    }
}
