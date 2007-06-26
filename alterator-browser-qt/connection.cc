#include <string>
#include <iostream>

#include <QFile>
#include <QTextStream>
#include <QStringList>

#include "connection.hh"
#include "global.hh"
#include "utils.hh"

#include "main_window.hh"
extern MainWindow *main_window;

Connection::Connection(QObject *parent):
    QThread(parent)
{
    destruction = false;
    islong_timer_id = 0;
    str2action["new"] = AlteratorRequestNew;
    str2action["close"] = AlteratorRequestClose;
    str2action["clean"] = AlteratorRequestClean;
    str2action["set"] = AlteratorRequestSet;
    str2action["create-event"] = AlteratorRequestEvent;
    str2action["splash"] = AlteratorRequestSplash;
    str2action["start"] = AlteratorRequestStart;
    str2action["stop"] = AlteratorRequestStop;
    str2action["messagebox"] = AlteratorRequestMessage;
    str2action["language"] = AlteratorRequestLanguage;
    str2action["retry"] = AlteratorRequestRetry;
    str2action["constraints-add"]   = AlteratorRequestCnstrAdd;
    str2action["constraints-clear"] = AlteratorRequestCnstrClear;
    str2action["constraints-apply"] = AlteratorRequestCnstrApply;

    connect(this, SIGNAL(started()), this, SLOT(startDelayedFinish()));
    connect(this, SIGNAL(finished()), this, SLOT(endDelayedFinish()));
}

Connection::~Connection()
{
    destruction = true;
    disconnect();
    if( islong_timer_id > 0 )
	killTimer(islong_timer_id);
    wait();
}

#if 0
void Connection::init()
{
	std::cout<<"(auth-request user \"qtbrowser\" password \"\" "
	<<"language \""<<createLangList().toLatin1().data()<<"\""
	<<")"<<std::endl;
	std::auto_ptr<alRequest> dom(readRequest());
	
	sessionId = dom->attrs_.value("session-id");
	userId = dom->attrs_.value("user");
//	std::cerr<<"session-id="<<sessionId.toLatin1().data()<<std::endl;
	
	parseAnswer(dom.get());
}
#endif

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
    while(!requests.isEmpty())
    {
	AlteratorAskInfo ask = requests.takeFirst();
	std::cout<< ask.request.toUtf8().data() << std::endl << std::flush;
	std::auto_ptr<alRequest> dom(readRequest());
	if( ask.flags & AlteratorRequestInit )
	{
	    sessionId = dom->attrs_.value("session-id");
	    userId = dom->attrs_.value("user");
	    //qDebug("session-id=%s", qPrintable(sessionId);
	}
	parseAnswer(dom.get(), ask.flags);
    }
}

void Connection::parseAnswer(alRequest *dom, AlteratorRequestFlags request_flags)
{
    AlteratorRequest request;
    request.flags = request_flags;
    QListIterator<alCommand*> it(dom->commands_);
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

AlteratorRequestParamData Connection::makeRequestParamData(AlteratorRequestParamDataType type, const QString& str)
{
    AlteratorRequestParamData data;
    switch( type )
    {
	case AlteratorRequestParamDataString:
	{
	    data.s = str; break;
	}
	case AlteratorRequestParamDataBool:
	{
	    data.b = str == "true"; break;
	}
	case AlteratorRequestParamDataInt:
	{
	    data.i = str.toInt(); break;
	}
	case AlteratorRequestParamDataOrientation:
	{
	    data.o = Utils::convertOrientation(str); break;
	}
	case AlteratorRequestParamDataButtons:
	{
	    QMessageBox::StandardButtons btns = AMessageBox::convertButtonList(str);
	    if(!btns) btns = QMessageBox::Ok;
	    data.buttons = btns;
	    break;
	}
	case AlteratorRequestParamDataUnknown:
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
	act.action = str2action[e.value("action")];

	switch( act.action )
	{
	    case AlteratorRequestNew:
	    {
		act.attr[AlteratorRequestParamWidgetId] = makeRequestParamData(AlteratorRequestParamDataString, e.value("widget-id"));
		act.attr[AlteratorRequestParamWidgetType] = makeRequestParamData(AlteratorRequestParamDataString, e.value("type"));
		act.attr[AlteratorRequestParamParentId] = makeRequestParamData(AlteratorRequestParamDataString, e.value("parent"));
		act.attr[AlteratorRequestParamWidth] = makeRequestParamData(AlteratorRequestParamDataInt, e.value("width"));
		act.attr[AlteratorRequestParamHeight] = makeRequestParamData(AlteratorRequestParamDataInt, e.value("height"));
		act.attr[AlteratorRequestParamOrientation] = makeRequestParamData(AlteratorRequestParamDataOrientation, e.value("orientation"));
		act.attr[AlteratorRequestParamSubType] = makeRequestParamData(AlteratorRequestParamDataString, e.value("sub-type"));
		act.attr[AlteratorRequestParamChecked] = makeRequestParamData(AlteratorRequestParamDataBool, e.value("checked"));
		act.attr[AlteratorRequestParamColumns] = makeRequestParamData(AlteratorRequestParamDataString, e.value("columns"));
		act.attr[AlteratorRequestParamRowSpan] = makeRequestParamData(AlteratorRequestParamDataInt, e.value("rowspan"));
		act.attr[AlteratorRequestParamColSpan] = makeRequestParamData(AlteratorRequestParamDataInt, e.value("colspan"));
		act.attr[AlteratorRequestParamTabIndex] = makeRequestParamData(AlteratorRequestParamDataInt, e.value("tab-index"));
		break;
	    }
	    case AlteratorRequestClose:
	    {
		act.attr[AlteratorRequestParamWidgetId] = makeRequestParamData(AlteratorRequestParamDataString, e.value("widget-id"));
		break;
	    }
	    case AlteratorRequestClean:
	    {
		act.attr[AlteratorRequestParamWidgetId] = makeRequestParamData(AlteratorRequestParamDataString, e.value("widget-id"));
		break;
	    }
	    case AlteratorRequestSet:
	    {
		act.attr[AlteratorRequestParamWidgetId] = makeRequestParamData(AlteratorRequestParamDataString, e.value("widget-id"));
		QString attr_name = e.value("name");
		act.attr[AlteratorRequestParamWidgetAttrName] = makeRequestParamData(AlteratorRequestParamDataString, attr_name);
		if( attr_name != "tab-index" )
		    act.attr[AlteratorRequestParamWidgetAttrValue] = makeRequestParamData(AlteratorRequestParamDataString, cmd->value_);
		else
		    act.attr[AlteratorRequestParamTabIndex] = makeRequestParamData(AlteratorRequestParamDataInt, cmd->value_);
		break;
	    }
	    case AlteratorRequestEvent:
	    {
		act.attr[AlteratorRequestParamWidgetId] = makeRequestParamData(AlteratorRequestParamDataString, e.value("widget-id"));
		act.attr[AlteratorRequestParamEventValue] = makeRequestParamData(AlteratorRequestParamDataString, cmd->value_);
		break;
	    }
	    case AlteratorRequestSplash:
	    {
		act.attr[AlteratorRequestParamSplashMessage] = makeRequestParamData(AlteratorRequestParamDataString, cmd->value_);
		break;
	    }
	    case AlteratorRequestStart:
	    {
		act.attr[AlteratorRequestParamWidgetId] = makeRequestParamData(AlteratorRequestParamDataString, e.value("widget-id"));
		break;
	    }
	    case AlteratorRequestStop:
	    {
		act.attr[AlteratorRequestParamWidgetId] = makeRequestParamData(AlteratorRequestParamDataString, e.value("widget-id"));
		break;
	    }
	    case AlteratorRequestMessage:
	    {
		act.attr[AlteratorRequestParamMessageType] = makeRequestParamData(AlteratorRequestParamDataString, e.value("type"));
		act.attr[AlteratorRequestParamMessageTitle] = makeRequestParamData(AlteratorRequestParamDataString, e.value("title"));
		act.attr[AlteratorRequestParamMessage] = makeRequestParamData(AlteratorRequestParamDataString, e.value("message"));
		act.attr[AlteratorRequestParamButtons] = makeRequestParamData(AlteratorRequestParamDataButtons, e.value("buttons"));
		break;
	    }
	    case AlteratorRequestLanguage:
	    {
		act.attr[AlteratorRequestParamLanguage] = makeRequestParamData(AlteratorRequestParamDataString, cmd->value_);
		break;
	    }
	    case AlteratorRequestRetry:
	    {
		break;
	    }
	    case AlteratorRequestCnstrAdd:
	    {
		act.attr[AlteratorRequestParamCnstrName] = makeRequestParamData(AlteratorRequestParamDataString, e.value("name"));
		act.attr[AlteratorRequestParamCnstrType] = makeRequestParamData(AlteratorRequestParamDataString, e.value("type"));
		act.attr[AlteratorRequestParamCnstrParams] = makeRequestParamData(AlteratorRequestParamDataString, e.value("params"));
		break;
	    }
	    case AlteratorRequestCnstrClear:
	    {
		break;
	    }
	    case AlteratorRequestCnstrApply:
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
