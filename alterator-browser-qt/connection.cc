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
	AlteratorRequestAction action = getDocParser(it.next());
	QList<AlteratorRequestAction> next_actions;
	if( action.action == AlteratorRequestNew )
	{
	    while( it.hasNext() )
	    {
		AlteratorRequestAction action_next = getDocParser(it.next());
		if(action_next.action == AlteratorRequestSet
		    && action.attr[AltReqParamWId].s == action_next.attr[AltReqParamWId].s
		    && params_for_new.contains(action_next.attr[AltReqParamWAttrName].s) )
		{
		    if( action_next.attr[AltReqParamWAttrName].s == "type" )
			action.attr[AltReqParamWType] = makeRequestParamData(AltReqParamDataType, action_next.attr[AltReqParamWAttrValue].s);
		    else if( action_next.attr[AltReqParamWAttrName].s == "parent" )
			action.attr[AltReqParamWParentId] = makeRequestParamData(AltReqParamDataString, action_next.attr[AltReqParamWAttrValue].s);
		    else if( action_next.attr[AltReqParamWAttrName].s == "width" )
			action.attr[AltReqParamWWidth] = makeRequestParamData(AltReqParamDataInt, action_next.attr[AltReqParamWAttrValue].s);
		    else if( action_next.attr[AltReqParamWAttrName].s == "height" )
			action.attr[AltReqParamWHeight] = makeRequestParamData(AltReqParamDataInt, action_next.attr[AltReqParamWAttrValue].s);
		    else if( action_next.attr[AltReqParamWAttrName].s == "orientation" )
			action.attr[AltReqParamWOrientation] = makeRequestParamData(AltReqParamDataOrientation, action_next.attr[AltReqParamWAttrValue].s);
		    else if( action_next.attr[AltReqParamWAttrName].s == "sub-type" )
			action.attr[AltReqParamWSubType] = makeRequestParamData(AltReqParamDataString, action_next.attr[AltReqParamWAttrValue].s);
		    else if( action_next.attr[AltReqParamWAttrName].s == "checked" )
			action.attr[AltReqParamWChecked] = makeRequestParamData(AltReqParamDataBool, action_next.attr[AltReqParamWAttrValue].s);
		    else if( action_next.attr[AltReqParamWAttrName].s == "columns" )
			action.attr[AltReqParamWColumns] = makeRequestParamData(AltReqParamDataString, action_next.attr[AltReqParamWAttrValue].s);
		    else if( action_next.attr[AltReqParamWAttrName].s == "rowspan" )
			action.attr[AltReqParamWRowSpan] = makeRequestParamData(AltReqParamDataInt, action_next.attr[AltReqParamWAttrValue].s);
		    else if( action_next.attr[AltReqParamWAttrName].s == "colspan" )
			action.attr[AltReqParamWColSpan] = makeRequestParamData(AltReqParamDataInt, action_next.attr[AltReqParamWAttrValue].s);
		    else if( action_next.attr[AltReqParamWAttrName].s == "tab-index" )
			action.attr[AltReqParamWTabIndex] = makeRequestParamData(AltReqParamDataInt, action_next.attr[AltReqParamWAttrValue].s);
		}
		else
		{
		    next_actions.append( action_next );
		    break;
		}
	    }
	}
	request.actions.append( action );
	foreach( AlteratorRequestAction act, next_actions )
	    request.actions.append( act );
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

/*
void Connection::setRequestActionParamData(QXmlAttributes &xmlattrs, AlteratorRequestAction &action, AlteratorRequestParamType ptype,AlteratorRequestParamDataType dtype, const QString &str)
{
    int pos = xmlattrs.index(str);
    if( pos >= 0 )
	action.attr[ptype] = makeRequestParamData(dtype, xmlattrs.value(pos));
}
*/

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
		act.attr[AltReqParamWId] = makeRequestParamData(AltReqParamDataString, e.value("widget-id"));
		act.attr[AltReqParamWType] = makeRequestParamData(AltReqParamDataType, e.value("type"));
		act.attr[AltReqParamWParentId] = makeRequestParamData(AltReqParamDataString, e.value("parent"));
		act.attr[AltReqParamWWidth] = makeRequestParamData(AltReqParamDataInt, e.value("width"));
		act.attr[AltReqParamWHeight] = makeRequestParamData(AltReqParamDataInt, e.value("height"));
		act.attr[AltReqParamWOrientation] = makeRequestParamData(AltReqParamDataOrientation, e.value("orientation"));
		act.attr[AltReqParamWSubType] = makeRequestParamData(AltReqParamDataString, e.value("sub-type"));
		act.attr[AltReqParamWChecked] = makeRequestParamData(AltReqParamDataBool, e.value("checked"));
		act.attr[AltReqParamWColumns] = makeRequestParamData(AltReqParamDataString, e.value("columns"));
		act.attr[AltReqParamWRowSpan] = makeRequestParamData(AltReqParamDataInt, e.value("rowspan"));
		act.attr[AltReqParamWColSpan] = makeRequestParamData(AltReqParamDataInt, e.value("colspan"));
		act.attr[AltReqParamWTabIndex] = makeRequestParamData(AltReqParamDataInt, e.value("tab-index"));
		break;
	    }
	    case AlteratorRequestClose:
	    {
		act.attr[AltReqParamWId] = makeRequestParamData(AltReqParamDataString, e.value("widget-id"));
		break;
	    }
	    case AlteratorRequestClean:
	    {
		act.attr[AltReqParamWId] = makeRequestParamData(AltReqParamDataString, e.value("widget-id"));
		break;
	    }
	    case AlteratorRequestSet:
	    {
		act.attr[AltReqParamWId] = makeRequestParamData(AltReqParamDataString, e.value("widget-id"));
		QString attr_name = e.value("name");
		act.attr[AltReqParamWAttrName] = makeRequestParamData(AltReqParamDataString, attr_name);
		act.attr[AltReqParamWAttrValue] = makeRequestParamData(AltReqParamDataString, cmd->value_);
		break;
	    }
	    case AlteratorRequestEvent:
	    {
		act.attr[AltReqParamWId] = makeRequestParamData(AltReqParamDataString, e.value("widget-id"));
		act.attr[AltReqParamEventValue] = makeRequestParamData(AltReqParamDataString, cmd->value_);
		break;
	    }
	    case AlteratorRequestSplash:
	    {
		act.attr[AltReqParamSplashMessage] = makeRequestParamData(AltReqParamDataString, cmd->value_);
		break;
	    }
	    case AlteratorRequestStart:
	    {
		act.attr[AltReqParamWId] = makeRequestParamData(AltReqParamDataString, e.value("widget-id"));
		break;
	    }
	    case AlteratorRequestStop:
	    {
		act.attr[AltReqParamWId] = makeRequestParamData(AltReqParamDataString, e.value("widget-id"));
		break;
	    }
	    case AlteratorRequestMessage:
	    {
		act.attr[AltReqParamMessageType] = makeRequestParamData(AltReqParamDataString, e.value("type"));
		act.attr[AltReqParamMessageTitle] = makeRequestParamData(AltReqParamDataString, e.value("title"));
		act.attr[AltReqParamMessage] = makeRequestParamData(AltReqParamDataString, e.value("message"));
		act.attr[AltReqParamButtons] = makeRequestParamData(AltReqParamDataButtons, e.value("buttons"));
		break;
	    }
	    case AlteratorRequestFile:
	    {
		act.attr[AltReqParamFileTitle] = makeRequestParamData(AltReqParamDataString, e.value("title"));
		act.attr[AltReqParamFileDir] = makeRequestParamData(AltReqParamDataString, e.value("dir"));
		act.attr[AltReqParamFileMask] = makeRequestParamData(AltReqParamDataString, e.value("mask"));
		act.attr[AltReqParamFileType] = makeRequestParamData(AltReqParamDataString, e.value("type"));
		break;
	    }
	    case AlteratorRequestLanguage:
	    {
		act.attr[AltReqParamLanguage] = makeRequestParamData(AltReqParamDataString, cmd->value_);
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
