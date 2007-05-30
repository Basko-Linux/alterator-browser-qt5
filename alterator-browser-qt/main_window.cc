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

#include "global.hh"
#include "widgets.hh"
#include "connection.hh"
#include "main_window.hh"
#include "messagebox.hh"
#include "constraints.hh"
#include "mailbox.hh"

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <QX11Info>
#endif

alWizardFace *wizard_face = 0;
MailBox *mailbox = 0;

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

MainWindow::MainWindow():
    MainWindow_t(0)
{
    qRegisterMetaType<Qt::Orientation>("Qt::Orientation");
    qRegisterMetaType<AlteratorRequest>("AlteratorRequest");
    loadStyleSheet();

    internal_splash = false;
    alterator_splash = false;
    emit_locker = 0; //wrong emit protector
    connection = 0;
    constraints = 0;
    qtranslator = 0;
    busy_timer_id = 0;
    started = false;
    detect_wm = false;

    QString language(getenv("LC_ALL"));
    if( language.isEmpty() )
	language = getenv("LC_MESSAGES");
    if( language.isEmpty() )
	language = getenv("LANG");
    if( language.isEmpty() )
	language = "C";
    changeLanguage(language);

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

    connect(qApp, SIGNAL(aboutToQuit()), this, SLOT(stop()));
    QTimer::singleShot(0, this, SLOT(start()));
}

MainWindow::~MainWindow()
{
}

void MainWindow::start()
{
    if( started ) return;
    started = true;

    AMessageBox::initButtonMap();

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
    socketDir.mkdir("alterator");

    socketPath += "/alterator/browser-sock";
    qDebug("socket path %s ...",qPrintable(socketPath));
    mailbox = new MailBox(socketPath, this);

    connect(connection, SIGNAL(alteratorRequest(const AlteratorRequest&)),
	    this, SLOT(onAlteratorRequest(const AlteratorRequest&)));

    connect(connection, SIGNAL(startLongRequest()),
	    this, SLOT(onStartBusySplash()));
    connect(connection, SIGNAL(stopLongRequest()),
	    this, SLOT(onStopBusySplash()));

    connection->init();
}

void MainWindow::stop()
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

/*
void MainWindow::showEvent(QShowEvent*)
{
    if( !started )
	QTimer::singleShot(0, this, SLOT(start()));
}
*/

bool MainWindow::haveWindowManager()
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

void MainWindow::setFullScreen(bool full)
{
    if( full )
    {
	geometry_ = geometry();
	setGeometry(QApplication::desktop()->geometry());
    }
    else
	setGeometry(geometry_);
}

void MainWindow::setHelpSource(const QString& str)
{
    help_browser->setHelpSource(str);
}

/*
void MainWindow::childEvent(QChildEvent *e)
{
    if( e->added() )
    {
	QObject *o = e->child();
	if( o->isWidgetType() )
	{
	    QWidget *w = qobject_cast<QWidget*>(e->child());
	    if( w )
	    {
		qDebug("MainWindow: child widget inserted");
	    }
	}
    }
}
*/

bool MainWindow::event(QEvent* e)
{
    if( e->type() == EVENT_HELP )
    {
	QTimer::singleShot(0, help_browser, SLOT(exec()));
    }
    return QMainWindow::event(e);
}

void MainWindow::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
	case Qt::Key_F1:
	{
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

void MainWindow::changeLanguage(const QString& language)
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

    if( qtranslator )
    {
	QCoreApplication::removeTranslator(qtranslator);
	delete qtranslator;
    }
    qtranslator = new QTranslator(this);
    qtranslator->load(QLibraryInfo::location(QLibraryInfo::TranslationsPath) + "/qt_"+QLocale::system().name());
    QCoreApplication::installTranslator(qtranslator);
    emit languageChanged();
}

void MainWindow::emitEvent(const QString& id,const QString& type, AlteratorRequestType request_type)
{
	if( emit_locker > 0 ) return;
	if( request_type == AlteratorRequestBlocking )
	{
//	    setEnabled(false);
	    ++emit_locker;
	}

	QString request = "(alterator-request action \"event\"";
	request += "name \""+type+"\"";//append type
	request += "widget-id \""+id+"\"";//append id
	
	//now collect a post data
	request += "\n state (";
	QMapIterator<QString,alWidget*> it(elements);
	while(it.hasNext())
	{
		it.next();
		QString data = it.value()->postData();
		if (!data.isEmpty())
			request += " \n("+it.key() + " "+data+ ")";
	}
	
	request += "))"; //close message

	connection->getDocument(request, request_type);
}

void MainWindow::onAlteratorRequest(const AlteratorRequest& request)
{
    QList<QString> tab_order_parents;
    QMap<QString, QMap<int,QWidget*> > tab_order_list;

    if( request.type == AlteratorRequestBlocking )
    {
	    --emit_locker;
//	    setEnabled(true);
    }

    for(AlteratorRequestActionList::const_iterator it = (request.actions).begin(); it != (request.actions).end(); it++)
    {
	AlteratorRequestActionInfo request = *it;
	switch( request.action )
	{
	    case AlteratorRequestNew:
	    {
		alWidget *new_wdg = onNewRequest(request.attr["widget-id"], request.attr["type"], request.attr["parent"],
		    request.attr["width"], request.attr["height"], Utils::convertOrientation(request.attr["orientation"]),
		    request.attr["sub-type"], request.attr["checked"] == "true", request.attr["columns"]);
		collectTabIndex(tab_order_parents, tab_order_list, new_wdg, request.attr["value"]);
		break;
	    }
	    case AlteratorRequestSet:
	    {
		QString attr_name = request.attr["name"];
		if( attr_name != "tab-index" )
		    onSetRequest(request.attr["widget-id"], attr_name, request.attr["value"]);
		else
		    collectTabIndex(tab_order_parents, tab_order_list, findAlWidgetById(request.attr["widget-id"]), request.attr["value"]);
		break;
	    }
	    case AlteratorRequestClose:
	    {
		onCloseRequest(request.attr["widget-id"]);
		break;
	    }
	    case AlteratorRequestClean:
	    {
		onCleanRequest(request.attr["widget-id"]);
		break;
	    }
	    case AlteratorRequestEvent:
	    {
		onEventRequest(request.attr["widget-id"], request.attr["value"]);
		break;
	    }
	    case AlteratorRequestSplash:
	    {
		onSplashMessageRequest(request.attr["value"]);
		break;
	    }
	    case AlteratorRequestStart:
	    {
		onStartRequest(request.attr["widget-id"]);
		break;
	    }
	    case AlteratorRequestStop:
	    {
		onStopRequest(request.attr["widget-id"]);
		break;
	    }
	    case AlteratorRequestMessage:
	    {
		onMessageBoxRequest(request.attr["type"], request.attr["title"],
			request.attr["message"], request.attr["buttons"]);
		break;
	    }
	    case AlteratorRequestLanguage:
	    {
		changeLanguage(request.attr["value"]);
		break;
	    }
	    case AlteratorRequestRetry:
	    {
		onRetryRequest();
		break;
	    }
	    case AlteratorRequestCnstrAdd:
	    {
		constraints->add(request.attr["name"], request.attr["type"], request.attr["params"]);
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
}

alWidget* MainWindow::onNewRequest(const QString &id, const QString &type, const QString &parent_id,
    const QString &width, const QString &height, Qt::Orientation orientation,  const QString &sub_type, bool checked,
    const QString &columns)
{
    /*
	qDebug("%s: id<%s> type<%s> parent_id<%s> orientation<%s> sub-type<%s> width<%s> height<%s> columns<%s>", __FUNCTION__,
	    qPrintable(id), qPrintable(type), qPrintable(parent_id),
	    orientation == Qt::Horizontal ? "-":"|", qPrintable(sub_type),
	    qPrintable(width), qPrintable(height), qPrintable(columns) );
	*/

	alWidget *new_widget = 0;
	if ("root" == type)
	{
	    const QString subtype = sub_type;
	    if ("popup" == subtype) //this is a dialog
	    {
	    	if(parent_id.isEmpty())
		    new_widget = new alMainWidget(id,"",orientation);
	    	else
		{
		    //new_widget = new alDialog(id,parent_id,orientation, width, height);
		    new_widget = new alDialog(id,parent_id,orientation, 0, 0);
		}
	    }
	    else
	    {
		    new_widget = new alBox(id,parent_id,orientation);
	    }
	}
	else if ("box" == type)         new_widget = new alBox(id,parent_id,orientation);
	else if ("vbox" == type)        new_widget = new alBox(id,parent_id,Qt::Vertical);
	else if ("hbox" == type)        new_widget = new alBox(id,parent_id,Qt::Horizontal);
	else if ("button" == type)      new_widget = new alButton(id,parent_id);
	else if ("radio" == type)       new_widget = new alRadio(id,parent_id);
	else if ("label" == type)       new_widget = new alLabel(id,parent_id);
	else if ("edit" == type)        new_widget = new alEdit(id,parent_id);
	else if ("textbox" == type)     new_widget = new alTextBox(id,parent_id);
	else if ("help-place" == type)  new_widget = new alHelpPlace(id,parent_id);
	else if ("groupbox" == type)    new_widget = new alGroupBox(id,parent_id,orientation, checked);
	else if ("gridbox" == type)     new_widget = new alGridBox(id,parent_id, columns);
	else if ("checkbox" == type)    new_widget = new alCheckBox(id,parent_id);
	else if ("tree" == type)        new_widget = new alTree(id,parent_id, columns);
	else if ("combobox" == type)    new_widget = new alComboBox(id,parent_id);
	else if ("tabbox" == type)      new_widget = new alTabBox(id,parent_id,orientation);
	else if ("tab-page" == type)    new_widget = new alTabPage(id,parent_id,orientation);
	else if ("progressbar" == type) new_widget = new alProgressBar(id,parent_id);
	else if ("slider" == type)      new_widget = new alSlider(id,parent_id);
	else if ("separator" == type)   new_widget = new alSeparator(id,parent_id,orientation);
	else if ("spacer" == type)      new_widget = new alSpacer(id,parent_id);
	else if ("spinbox" == type)     new_widget = new alSpinBox(id,parent_id);
	else if ("dateedit" == type)    new_widget = new alDateEdit(id,parent_id);
	else if ("timeedit" == type)    new_widget = new alTimeEdit(id,parent_id);
	else if ("listbox" == type)	new_widget = new alMultiListBox(id,parent_id,columns.toInt());
	else if ("slideshow" == type)	new_widget = new alSlideShow(id,parent_id);
	else if ("splitbox" == type)    new_widget = new alSplitBox(id,parent_id,orientation);
	else if ("wizardface" == type)
	{
	    if( wizard_face )
		new_widget = new alBox(id,parent_id,orientation);
	    else
		new_widget = wizard_face = new alWizardFace(id,parent_id,orientation);
	}
	else
	{
	    qDebug("Unknown widget \"%s\". Make box instead.", qPrintable(type));
	    new_widget = new alBox(id,parent_id,orientation);
	}

	//
	if( new_widget )
	{
	    switch( new_widget->type() )
	    {
//		case alWidget::Dialog:
//		    break;
		default:
		{
		    int w = width.toInt();
		    if( w > 0 ) new_widget->setAttr("width", width);
		    int h = height.toInt();
		    if( h > 0 ) new_widget->setAttr("height", height);
		}
	    }
	}
    return new_widget;
}

void MainWindow::onCloseRequest(const QString& id)
{
	if( elements.contains(id) )
	{
	    elements.take(id)->destroyLater();
	}
}

void MainWindow::onCleanRequest(const QString& id)
{
	if( !elements.contains(id) )
	    return;

	alWidget *el = elements[id];
	
	QLayout* layout = el->getViewLayout();
	if( layout )
	{
	    for (int i = 0; i < layout->count(); ++i)
		delete layout->takeAt(i);
	}
	
	QList<alWidget*> children = el->findChildren<alWidget*>();
	if( children.size() > 0 )
	{
	    QListIterator<alWidget *> it(children);
	    while( it.hasNext() )
	    {
		alWidget *aw = it.next();
		aw->show(false);
		aw->destroyLater();
	    }
	}
}

void MainWindow::onSetRequest(const QString& id,const QString& attr,const QString& value)
{
	//qDebug("%s: id<%s> attr<%s> value<%s>", __FUNCTION__, id.toLatin1().data(), attr.toLatin1().data(), value.toLocal8Bit().data());
	if (elements.contains(id))
	{
	    ++emit_locker;
	    elements[id]->setAttr(attr,value);
	    --emit_locker;
	}
}

void MainWindow::onStartRequest(const QString& id)
{
	if (!elements.contains(id)) return;
	alWidget *aw = elements[id];
	if( aw )
	{
	    if( aw->type() == alWidget::MainWidget  )
	    {
		alMainWidget *m = qobject_cast<alMainWidget*>(aw);
		if(m)  m->start();
	    }
	    else if ( aw->type() == alWidget::Dialog  )
	    {
		alDialog *d = qobject_cast<alDialog*>(aw);
		if(d) d->start();
	    }
	}
}

void MainWindow::onStopRequest(const QString& id)
{
	if (!elements.contains(id))
	    return;
	alWidget *aw = elements[id];
	if( aw )
	{
	    if( aw->type() == alWidget::MainWidget  )
	    {
		alMainWidget *m = qobject_cast<alMainWidget*>(aw);
		if(m)  m->stop();
	    }
	    else if ( aw->type() == alWidget::Dialog  )
	    {
		alDialog *d = qobject_cast<alDialog*>(aw);
		if(d) d->stop();
	    }
	}
}

void MainWindow::onEventRequest(const QString& id,const QString& value)
{
	if (!elements.contains(id))
	    return;
	else
	    elements[id]->registerEvent(value);
}

void MainWindow::onMessageBoxRequest(const QString& type, const QString& title,  const QString& message, const QString& buttons)
{
    QWidget *parent = QApplication::activeWindow();
    AMsgBox msgbox(type, title, message, buttons, parent);
    //qDebug("AMsgBox exec");
    const QString answer = AMessageBox::unconvertButton((QMessageBox::StandardButton)msgbox.exec());
    connection->getDocument(answer);
}

void MainWindow::splashStart(void)
{
	if (splash) return;

	QPixmap px(250,50);
	px.fill(QApplication::palette().color(QPalette::Background));

	QPainter painter(&px);
	painter.drawLine(1, 2, 1, px.height()-2);
	painter.drawLine(2, 1, px.width()-2,1);
	painter.drawLine(px.width()-1, 2, px.width()-1, px.height()-2);
	painter.drawLine(2, px.height()-1, px.width()-2, px.height()-1);

	splash = new QSplashScreen(this, px, Qt::WindowStaysOnTopHint);
	//splash->setWindowModality(Qt::WindowModal);
	splash->setWindowModality(Qt::ApplicationModal);
	splash->show();
}

void MainWindow::onSplashMessageRequest(const QString& msg)
{
	if (msg.isEmpty())
	{
	    alterator_splash = false;
	    if( !internal_splash && splash )
		delete splash;
	}
	else
	{
	    alterator_splash = true;
	    if (!splash)
		splashStart();
	    splash->showMessage(msg);
	}
}

void MainWindow::onInternalSplashMessage(const QString& msg)
{
	if (msg.isEmpty())
	{
	    internal_splash = false;
	    if( !alterator_splash && splash )
		delete splash;
	}
	else
	{
	    internal_splash = true;
	    if (!splash)
		splashStart();
	    if( !alterator_splash )
		splash->showMessage(msg);
	}
}

void MainWindow::getDocument(const QString& request)
{
    connection->getDocument(request);
}

void MainWindow::onStartBusySplash()
{
    setCursor(Qt::WaitCursor);
}

void MainWindow::onStopBusySplash()
{
    if( busy_timer_id > 0 )
	killTimer(busy_timer_id);
    busy_timer_id = startTimer(500);
}

void MainWindow::timerEvent(QTimerEvent *e)
{
    if( e->timerId() == busy_timer_id )
    {
	killTimer(busy_timer_id);
	busy_timer_id = 0;
	unsetCursor();
    }
}

void MainWindow::onRetryRequest()
{
    QTimer::singleShot(50, this,SLOT(doRetry()));
}

void MainWindow::doRetry()
{
    connection->getDocument("(alterator-request action \"re-get\")");
}

void MainWindow::loadStyleSheet()
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
