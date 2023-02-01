#include <string>
#include <iostream>

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QCoreApplication>

#include "connection.hh"
#include "global.hh"
#include "utils.hh"
#include "enums.hh"

#include "browser.hh"

Connection::Connection(QObject *parent):
    QThread(parent)
{
    destruction = false;
    is_processing = false;
#ifdef QTHREAD_EXEC_WORKAROUND
    thread_exec = false;
    thread_exec_result = 0;
#endif

    islong_timer = new QTimer(this);
    islong_timer->setSingleShot(true);
    islong_timer->setInterval(500);
    connect(islong_timer, SIGNAL(timeout()), this, SLOT(checkDelayedFinish()));
    connect(this, SIGNAL(startProcessing()), islong_timer, SLOT(start()));

    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(prepareQuit()));
}

Connection::~Connection()
{
}

void Connection::init()
{
	getDocument(QString(), AlteratorRequestInit);
}

QString Connection::makeRequest(const QString& content)
{
	QString out;
	QTextStream s(&out);
	s<<"(auth-request user \""<<userId<<"\" session-id "
	 <<"\""<<sessionId<<"\""
	 <<" content "<<content<<")";
       return out;
}

QString Connection::makeInitRequest()
{
    QString out;
    QTextStream s(&out);
    s<<"(auth-request user \"qtbrowser\" password \"\" "
    <<"language \""<<createLangList().toLatin1().data()<<"\""
    <<")";
    return out;
}

void Connection::getDocument(const QString &content, AlteratorRequestFlags ask_flags)
{
    if( destruction ) return;

    AlteratorAskInfo ask;
    ask.flags = ask_flags;
    if( ask_flags & AlteratorRequestInit )
	ask.request = makeInitRequest();
    else
	ask.request = makeRequest(content);

    asks_lock.lock();
    asks.enqueue(ask);
    asks_lock.unlock();

    if(isRunning())
    {
	if(!is_processing)
#ifdef QTHREAD_EXEC_WORKAROUND
	    myQuit();
#else
	    quit();
#endif
    }
    else
	start();
}

/* Guess value of current locale from value of the environment variables.  */
QByteArray guess_locale_value(void)
{
       /* The highest priority value is the `LANGUAGE' environment
          variable.  But we don't use the value if the currently selected
          locale is the C locale.  This is a GNU extension.  */
       QByteArray language(qgetenv("LANGUAGE"));
       language = language.trimmed();

       /* We have to proceed with the POSIX methods of looking to `LC_ALL',
          `LC_xxx', and `LANG'.  On some systems this can be done by the
          `setlocale' function itself.  */
       QByteArray retval(::setlocale(LC_MESSAGES, NULL));

       /* Ignore LANGUAGE if the locale is set to "C" because
          1. "C" locale usually uses the ASCII encoding, and most international
          messages use non-ASCII characters. These characters get displayed
          as question marks (if using glibc's iconv()) or as invalid 8-bit
          characters (because other iconv()s refuse to convert most non-ASCII
          characters to ASCII). In any case, the output is ugly.
          2. The precise output of some programs in the "C" locale is specified
          by POSIX and should not depend on environment variables like
          "LANGUAGE".  We allow such programs to use gettext().  */
       return (!language.isEmpty() && retval != "C")? language : retval;
}

QString Connection::createLangList()
{
    QString lang = QLatin1String(guess_locale_value());
    if( lang.isEmpty() )
	lang = QStringLiteral("POSIX");
    QStringList lst = lang.split(QLatin1Char(':'), Qt::SkipEmptyParts);
    QStringList lst2;
    QStringListIterator it(lst);
    while(it.hasNext())
    {
	QString item = it.next();
	lst2.append(item.replace(QRegularExpression(QStringLiteral("\\..*")),QString()));
    }
    return lst2.join(QLatin1Char(';'));
}

void Connection::run()
{
    while( !destruction )
    {
	is_processing = true;
	Q_EMIT startProcessing();
	while(!asks.isEmpty())
	{
	    if( destruction ) break;
	    asks_lock.lock();
	    AlteratorAskInfo ask = asks.dequeue();
	    asks_lock.unlock();
	    std::cout<< ask.request.toUtf8().data() << std::endl << std::flush;
	    alRequest dom = readRequest();
	    if( ask.flags & AlteratorRequestInit )
	    {
		sessionId = dom.attrs_.value(QStringLiteral("session-id"));
		userId = dom.attrs_.value(QStringLiteral("user"));
		//qDebug("session-id=%s", qPrintable(sessionId);
	    }
	    parseAnswer(dom, ask.flags);
	}
	is_processing = false;
	if( !destruction ) {
	    Q_EMIT stopLongRequest();
	}
#ifdef QTHREAD_EXEC_WORKAROUND
	if( myExec() != 0 )
#else
	if( exec() != 0 )
#endif
	{
	    destruction = true;
	    break;
	}
    }
}

void Connection::parseAnswer(const alRequest &dom, AlteratorRequestFlags request_flags)
{
    AlteratorRequest request;
    request.flags = request_flags;
    QListIterator<alCommand*> it(dom.commands_);
    while(it.hasNext())
    {
	request.actions.append( getDocParser(it.next()) );
    }
    alterator_requests_lock.lock();
    alterator_requests.enqueue(request);
    alterator_requests_lock.unlock();
    Q_EMIT alteratorRequests();
}

void Connection::checkDelayedFinish()
{
    if( is_processing && !destruction )
        Q_EMIT startLongRequest();
}

bool Connection::setRequestActionParamData(QXmlAttributes &xmlattrs, const QString &xmlattrname, AlteratorRequestAction &action, const QString &attrname, AlteratorRequestParamDataType dtype)
{
    int pos = xmlattrs.index(xmlattrname);
    if( pos >= 0 )
    {
	action.attr.insert(attrname, makeRequestParamData(dtype, xmlattrs.value(pos)));
	return true;
    }
    else
	return false;
}

void Connection::setRequestActionParamString(const QString &value, AlteratorRequestAction &action, const QString &attrname)
{
    action.attr.insert(attrname, makeRequestParamData(AltReqParamDataString, value));
}

AlteratorRequestParamData Connection::makeRequestParamData(AlteratorRequestParamDataType type, const QString& str)
{
    AlteratorRequestParamData data;
    data.s = str;
    switch( type )
    {
	case AltReqParamDataType:
	{
	    data.t = g_enums->strToWidget(str.toLatin1());
	    break;
	}
	case AltReqParamDataString:
	{
	    if( data.s.isEmpty() )
		data.s = str;
	    break;
	}
	case AltReqParamDataBool:
	{
	    data.b = str == QStringLiteral("true"); break;
	}
	case AltReqParamDataInt:
	{
	    data.i = str.toInt(); break;
	}
	case AltReqParamDataOrientation:
	{
	    data.o = Utils::convertOrientation(str); break;
	}
	case AltReqParamDataButtons:
	{
	    QDialogButtonBox::StandardButtons btns = MsgBox::convertButtonList(str);
	    if(!btns) btns = QDialogButtonBox::Ok;
	    data.buttons = btns;
	    break;
	}
	case AltReqParamDataUnknown:
	{
	    qDebug("Unknown AlteratorRequestParamDataType.");
	    break;
	}
/*	default:
	    break; */
    }
    return data;
}

AlteratorRequestAction Connection::getDocParser(alCommand *cmd)
{
	QXmlAttributes e = cmd->attrs_;
	AlteratorRequestAction act;
	act.action = g_enums->strToRequestAction(e.value(QStringLiteral("action")).toLatin1());

	switch( act.action )
	{
	    case AlteratorRequestNew:
	    {
		int ecount = e.count();
		for(int i=0; i < ecount; i++)
		{
		    if( e.type(i) == QStringLiteral("CDATA") )
		    {
			QString name= e.qName(i);
			     if( QStringLiteral("type") == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataType, e.value(i)));
			else if( QStringLiteral("width") == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataInt, e.value(i)));
			else if( QStringLiteral("height") == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataInt, e.value(i)));
			else if( QStringLiteral("rowspan") == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataInt, e.value(i)));
			else if( QStringLiteral("colspan") == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataInt, e.value(i)));
			else if( QStringLiteral("tab-index") == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataInt, e.value(i)));
			else if( QStringLiteral("columns") == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataInt, e.value(i)));
			else if( QStringLiteral("checked") == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataBool, e.value(i)));
			else if( QStringLiteral("orientation") == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataOrientation, e.value(i)));
			else
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataString, e.value(i)));
		    }
		}
		act.attr.remove(QStringLiteral("action"));
		act.attr.remove(QStringLiteral("xml:space"));
		if( !act.attr.contains(QStringLiteral("orientation")) )
		    act.attr.insert(QStringLiteral("orientation"), makeRequestParamData(AltReqParamDataOrientation, QStringLiteral("__undefined__")));
		break;
	    }
	    case AlteratorRequestClose:
	    {
		setRequestActionParamData(e, QStringLiteral("widget-id"), act, QStringLiteral("widget-id"), AltReqParamDataString);
		break;
	    }
	    case AlteratorRequestClean:
	    {
		setRequestActionParamData(e, QStringLiteral("widget-id"), act, QStringLiteral("widget-id"), AltReqParamDataString);
		break;
	    }
	    case AlteratorRequestSet:
	    {
		if( setRequestActionParamData(e, QStringLiteral("widget-id"), act, QStringLiteral("widget-id"), AltReqParamDataString)
		    && setRequestActionParamData(e, QStringLiteral("name"), act, QStringLiteral("attr-name"), AltReqParamDataString) )
			setRequestActionParamString(cmd->value_, act, QStringLiteral("attr-value"));
		break;
	    }
	    case AlteratorRequestEvent:
	    {
		if( setRequestActionParamData(e, QStringLiteral("widget-id"), act, QStringLiteral("widget-id"), AltReqParamDataString) )
		    setRequestActionParamString(cmd->value_, act, QStringLiteral("event-value"));
		break;
	    }
	    case AlteratorRequestSplash:
	    {
		setRequestActionParamString(cmd->value_, act, QStringLiteral("splash-message"));
		break;
	    }
	    case AlteratorRequestStart:
	    {
		setRequestActionParamData(e, QStringLiteral("widget-id"), act, QStringLiteral("widget-id"), AltReqParamDataString);
		break;
	    }
	    case AlteratorRequestStop:
	    {
		setRequestActionParamData(e, QStringLiteral("widget-id"), act, QStringLiteral("widget-id"), AltReqParamDataString);
		break;
	    }
	    case AlteratorRequestMessage:
	    {
		if( setRequestActionParamData(e, QStringLiteral("message"), act, QStringLiteral("message"), AltReqParamDataString)
		    && setRequestActionParamData(e, QStringLiteral("buttons"), act, QStringLiteral("buttons"), AltReqParamDataButtons)
		    && setRequestActionParamData(e, QStringLiteral("type"), act, QStringLiteral("message-type"), AltReqParamDataString) )
			setRequestActionParamData(e, QStringLiteral("title"), act, QStringLiteral("message-title"), AltReqParamDataString);
		break;
	    }
	    case AlteratorRequestFile:
	    {
		setRequestActionParamData(e, QStringLiteral("title"), act, QStringLiteral("file-title"), AltReqParamDataString);
		setRequestActionParamData(e, QStringLiteral("dir"), act, QStringLiteral("file-dir"), AltReqParamDataString);
		setRequestActionParamData(e, QStringLiteral("mask"), act, QStringLiteral("file-mask"), AltReqParamDataString);
		setRequestActionParamData(e, QStringLiteral("type"), act, QStringLiteral("file-type"), AltReqParamDataString);
		break;
	    }
	    case AlteratorRequestLanguage:
	    {
		setRequestActionParamString(cmd->value_, act, QStringLiteral("language"));
		break;
	    }
	    case AlteratorRequestRetry:
	    {
		break;
	    }
	    case AlteratorRequestUnknown:
	    {
		qDebug("Unknown alterator request action \"%s\".", qPrintable(e.value(QStringLiteral("action"))));
		break;
	    }
/*
	    default:
		break;
*/
	}
	return act;
}

void Connection::prepareQuit()
{
    destruction = true;
#ifdef QTHREAD_EXEC_WORKAROUND
    myExit(1);
#else
    exit(1);
#endif
}

QQueue<AlteratorRequest> Connection::getRequests()
{
    alterator_requests_lock.lock();
    QQueue<AlteratorRequest> ret(alterator_requests);
    alterator_requests.clear();
    alterator_requests_lock.unlock();
    return ret;
}

#ifdef QTHREAD_EXEC_WORKAROUND
void Connection::myQuit()
{
    thread_exec_result = 0;
    thread_exec = false;
}

void Connection::myExit(int ret)
{
    thread_exec_result = ret;
    thread_exec = false;
}

int Connection::myExec()
{
    thread_exec = true;
    while( thread_exec )
    {
	msleep(256);
    }
    return thread_exec_result;
}

#endif
