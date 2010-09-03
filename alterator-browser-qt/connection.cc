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
    islong_timer_id = 0;
#ifdef QTHREAD_EXEC_WORKAROUND
    thread_exec = false;
    thread_exec_result = 0;
#endif

    //connect(this, SIGNAL(started()), this, SLOT(startDelayedFinish()));
    //connect(this, SIGNAL(finished()), this, SLOT(endDelayedFinish()));
    connect(QCoreApplication::instance(), SIGNAL(aboutToQuit()), this, SLOT(prepareQuit()));
}

Connection::~Connection()
{
}

void Connection::init()
{
	getDocument("", AlteratorRequestInit);
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
static const char *guess_locale_value(void)
{
       const char *language;
       const char *retval;

       /* The highest priority value is the `LANGUAGE' environment
          variable.  But we don't use the value if the currently selected
          locale is the C locale.  This is a GNU extension.  */
       language = ::getenv("LANGUAGE");
       if (language != NULL && language[0] == '\0')
               language = NULL;

       /* We have to proceed with the POSIX methods of looking to `LC_ALL',
          `LC_xxx', and `LANG'.  On some systems this can be done by the
          `setlocale' function itself.  */
       retval = ::setlocale(LC_MESSAGES, NULL);

       /* Ignore LANGUAGE if the locale is set to "C" because
          1. "C" locale usually uses the ASCII encoding, and most international
          messages use non-ASCII characters. These characters get displayed
          as question marks (if using glibc's iconv()) or as invalid 8-bit
          characters (because other iconv()s refuse to convert most non-ASCII
          characters to ASCII). In any case, the output is ugly.
          2. The precise output of some programs in the "C" locale is specified
          by POSIX and should not depend on environment variables like
          "LANGUAGE".  We allow such programs to use gettext().  */
       return language != NULL
               && ::strcmp(retval, "C") ? language : retval;
}

QString Connection::createLangList()
{
    QString lang(guess_locale_value());
    if( lang.isEmpty() )
	lang = "POSIX";
    QStringList lst = lang.split( ":", QString::SkipEmptyParts);
    QStringList lst2;
    QStringListIterator it(lst);
    while(it.hasNext())
    {
	QString item = it.next();
	lst2.append(item.replace(QRegExp("\\..*"),""));
    }
    return lst2.join(";");
}

void Connection::run()
{
    while( !destruction )
    {
	startDelayedFinish();
	is_processing = true;
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
		sessionId = dom.attrs_.value("session-id");
		userId = dom.attrs_.value("user");
		//qDebug("session-id=%s", qPrintable(sessionId);
	    }
	    parseAnswer(dom, ask.flags);
	}
	is_processing = false;
	if( !destruction )
	    endDelayedFinish();
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
    emit alteratorRequests();
}

void Connection::startDelayedFinish()
{
    if( islong_timer_id > 0 )
	killTimer(islong_timer_id);
    islong_timer_id = startTimer(500);
}

void Connection::timerEvent(QTimerEvent *e)
{
    if( e->timerId() == islong_timer_id )
    {
	killTimer(islong_timer_id);
	islong_timer_id = 0;
	if( is_processing )
	    emit startLongRequest();
    }
}

void Connection::endDelayedFinish()
{
    emit stopLongRequest();
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
	    data.t = enums->strToWidget(str);
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
	    data.b = str == "true"; break;
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
	act.action = enums->strToRequestAction(e.value("action"));

	switch( act.action )
	{
	    case AlteratorRequestNew:
	    {
		int ecount = e.count();
		for(int i=0; i < ecount; i++)
		{
		    if( e.type(i) == "CDATA" )
		    {
			QString name= e.qName(i);
			     if( "type" == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataType, e.value(i)));
			else if( "width" == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataInt, e.value(i)));
			else if( "height" == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataInt, e.value(i)));
			else if( "rowspan" == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataInt, e.value(i)));
			else if( "colspan" == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataInt, e.value(i)));
			else if( "tab-index" == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataInt, e.value(i)));
			else if( "columns" == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataInt, e.value(i)));
			else if( "checked" == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataBool, e.value(i)));
			else if( "orientation" == name )
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataOrientation, e.value(i)));
			else
			    act.attr.insert(name, makeRequestParamData(AltReqParamDataString, e.value(i)));
		    }
		}
		act.attr.remove("action");
		act.attr.remove("xml:space");
		if( !act.attr.contains("orientation") )
		    act.attr.insert("orientation", makeRequestParamData(AltReqParamDataOrientation, "__undefined__"));
		break;
	    }
	    case AlteratorRequestClose:
	    {
		setRequestActionParamData(e, "widget-id", act, "widget-id", AltReqParamDataString);
		break;
	    }
	    case AlteratorRequestClean:
	    {
		setRequestActionParamData(e, "widget-id", act, "widget-id", AltReqParamDataString);
		break;
	    }
	    case AlteratorRequestSet:
	    {
		if( setRequestActionParamData(e, "widget-id", act, "widget-id", AltReqParamDataString)
		    && setRequestActionParamData(e, "name", act, "attr-name", AltReqParamDataString) )
			setRequestActionParamString(cmd->value_, act, "attr-value");
		break;
	    }
	    case AlteratorRequestEvent:
	    {
		if( setRequestActionParamData(e, "widget-id", act, "widget-id", AltReqParamDataString) )
		    setRequestActionParamString(cmd->value_, act, "event-value");
		break;
	    }
	    case AlteratorRequestSplash:
	    {
		setRequestActionParamString(cmd->value_, act, "splash-message");
		break;
	    }
	    case AlteratorRequestStart:
	    {
		setRequestActionParamData(e, "widget-id", act, "widget-id", AltReqParamDataString);
		break;
	    }
	    case AlteratorRequestStop:
	    {
		setRequestActionParamData(e, "widget-id", act, "widget-id", AltReqParamDataString);
		break;
	    }
	    case AlteratorRequestMessage:
	    {
		if( setRequestActionParamData(e, "message", act, "message", AltReqParamDataString)
		    && setRequestActionParamData(e, "buttons", act, "buttons", AltReqParamDataButtons)
		    && setRequestActionParamData(e, "type", act, "message-type", AltReqParamDataString) )
			setRequestActionParamData(e, "title", act, "message-title", AltReqParamDataString);
		break;
	    }
	    case AlteratorRequestFile:
	    {
		setRequestActionParamData(e, "title", act, "file-title", AltReqParamDataString);
		setRequestActionParamData(e, "dir", act, "file-dir", AltReqParamDataString);
		setRequestActionParamData(e, "mask", act, "file-mask", AltReqParamDataString);
		setRequestActionParamData(e, "type", act, "file-type", AltReqParamDataString);
		break;
	    }
	    case AlteratorRequestLanguage:
	    {
		setRequestActionParamString(cmd->value_, act, "language");
		break;
	    }
	    case AlteratorRequestRetry:
	    {
		break;
	    }
	    case AlteratorRequestUnknown:
	    {
		qDebug("Unknown alterator request action \"%s\".", qPrintable(e.value("action")));
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
    if( islong_timer_id > 0 )
	killTimer(islong_timer_id);
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
