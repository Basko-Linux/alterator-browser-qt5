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
#ifndef USE_CONN_NO_THREAD
    QThread(parent)
#else
    QObject(parent)
#endif
{
#ifndef USE_CONN_NO_THREAD
#else
    is_running = false;
#endif
    destruction = false;
    islong_timer_id = 0;

    connect(this, SIGNAL(started()), this, SLOT(startDelayedFinish()));
    connect(this, SIGNAL(finished()), this, SLOT(endDelayedFinish()));
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

void Connection::getDocument(const QString &content, AlteratorRequestFlags request_flags)
{
    if( destruction ) return;
    AlteratorAskInfo ask;
    ask.flags = request_flags;
    if( request_flags & AlteratorRequestInit )
	ask.request = makeInitRequest();
    else
	ask.request = makeRequest(content);
    requests.append(ask);
    if(!isRunning())
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
#ifndef USE_CONN_NO_THREAD
#else
    is_running = true;
#endif
    while(!requests.isEmpty())
    {
	if( destruction ) break;
	AlteratorAskInfo ask = requests.takeFirst();
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
#ifndef USE_CONN_NO_THREAD
#else
    is_running = false;
#endif
}

void Connection::parseAnswer(const alRequest &dom, AlteratorRequestFlags request_flags)
{
    QList<QString> params_for_new;
    params_for_new << "type" << "parent" << "width" << "height" << "orientation" << "sub-type" << "checked" << "columns" << "rowspan" << "colspan" << "tab-index";
    AlteratorRequest request;
    request.flags = request_flags;
    QListIterator<alCommand*> it(dom.commands_);
    while(it.hasNext())
    {
	request.actions.append( getDocParser(it.next()) );
    }
    emit alteratorRequest(request);
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
	if( isRunning() )
	    emit startLongRequest();
    }
}

void Connection::endDelayedFinish()
{
    emit stopLongRequest();
}

void Connection::setRequestActionParamData(QXmlAttributes &xmlattrs, const QString &xmlattrname, AlteratorRequestAction &action, const QString &attrname, AlteratorRequestParamDataType dtype)
{
    int pos = xmlattrs.index(xmlattrname);
    if( pos >= 0 )
	action.attr.insert(attrname, makeRequestParamData(dtype, xmlattrs.value(pos)));
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
		act.attr.insert("widget-id", makeRequestParamData(AltReqParamDataString, e.value("widget-id")));
		act.attr.insert("widget-type", makeRequestParamData(AltReqParamDataType, e.value("type")));
		act.attr.insert("parent-id", makeRequestParamData(AltReqParamDataString, e.value("parent")));
		act.attr.insert("width", makeRequestParamData(AltReqParamDataInt, e.value("width")));
		act.attr.insert("height", makeRequestParamData(AltReqParamDataInt, e.value("height")));
		act.attr.insert("orientation", makeRequestParamData(AltReqParamDataOrientation, e.value("orientation")));
		act.attr.insert("sub-type", makeRequestParamData(AltReqParamDataString, e.value("sub-type")));
		act.attr.insert("checked", makeRequestParamData(AltReqParamDataBool, e.value("checked")));
		act.attr.insert("columns", makeRequestParamData(AltReqParamDataString, e.value("columns")));
		act.attr.insert("rowspan", makeRequestParamData(AltReqParamDataInt, e.value("rowspan")));
		act.attr.insert("colspan", makeRequestParamData(AltReqParamDataInt, e.value("colspan")));
		setRequestActionParamData(e, "tab-index", act, "tab-index", AltReqParamDataInt);
		break;
	    }
	    case AlteratorRequestClose:
	    {
		act.attr.insert("widget-id", makeRequestParamData(AltReqParamDataString, e.value("widget-id")));
		break;
	    }
	    case AlteratorRequestClean:
	    {
		act.attr.insert("widget-id", makeRequestParamData(AltReqParamDataString, e.value("widget-id")));
		break;
	    }
	    case AlteratorRequestSet:
	    {
		act.attr.insert("widget-id", makeRequestParamData(AltReqParamDataString, e.value("widget-id")));
		act.attr.insert("attr-name", makeRequestParamData(AltReqParamDataString, e.value("name")));
		act.attr.insert("attr-value", makeRequestParamData(AltReqParamDataString, cmd->value_));
		break;
	    }
	    case AlteratorRequestEvent:
	    {
		act.attr.insert("widget-id", makeRequestParamData(AltReqParamDataString, e.value("widget-id")));
		act.attr.insert("event-value", makeRequestParamData(AltReqParamDataString, cmd->value_));
		break;
	    }
	    case AlteratorRequestSplash:
	    {
		act.attr.insert("splash-message", makeRequestParamData(AltReqParamDataString, cmd->value_));
		break;
	    }
	    case AlteratorRequestStart:
	    {
		act.attr.insert("widget-id", makeRequestParamData(AltReqParamDataString, e.value("widget-id")));
		break;
	    }
	    case AlteratorRequestStop:
	    {
		act.attr.insert("widget-id", makeRequestParamData(AltReqParamDataString, e.value("widget-id")));
		break;
	    }
	    case AlteratorRequestMessage:
	    {
		act.attr.insert("message-type", makeRequestParamData(AltReqParamDataString, e.value("type")));
		act.attr.insert("message-title", makeRequestParamData(AltReqParamDataString, e.value("title")));
		act.attr.insert("message", makeRequestParamData(AltReqParamDataString, e.value("message")));
		act.attr.insert("buttons", makeRequestParamData(AltReqParamDataButtons, e.value("buttons")));
		break;
	    }
	    case AlteratorRequestFile:
	    {
		act.attr.insert("file-title", makeRequestParamData(AltReqParamDataString, e.value("title")));
		act.attr.insert("file-dir", makeRequestParamData(AltReqParamDataString, e.value("dir")));
		act.attr.insert("file-mask", makeRequestParamData(AltReqParamDataString, e.value("mask")));
		act.attr.insert("file-type", makeRequestParamData(AltReqParamDataString, e.value("type")));
		break;
	    }
	    case AlteratorRequestLanguage:
	    {
		act.attr.insert("language", makeRequestParamData(AltReqParamDataString, cmd->value_));
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
    if( islong_timer_id > 0 )
	killTimer(islong_timer_id);
    quit();
}
