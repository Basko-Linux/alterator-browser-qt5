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

void Connection::getDocument(const QString &content)
{
    if( destruction ) return;
    requests.append(makeRequest(content));
    if(!isRunning())
	start();
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

void Connection::parseAnswer(alRequest *dom)
{
    QListIterator<alCommand*> it(dom->commands_);
    while(it.hasNext())
    {
	getDocParser(it.next());
    }
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
	std::cout<< requests.takeFirst().toUtf8().data() << std::endl << std::flush;
	std::auto_ptr<alRequest> dom(readRequest());
	parseAnswer(dom.get());
    }
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

void Connection::getDocParser(alCommand *cmd)
{
	QXmlAttributes e = cmd->attrs_;
	AlteratorRequestInfo request;
	request.type = AlteratorRequestDefault;
	request.action = str2action[e.value("action")];
	switch( request.action )
	{
	    case AlteratorRequestNew:
	    {
		request.attr["widget-id"] = e.value("widget-id");
		request.attr["type"] = e.value("type");
		request.attr["parent"] = e.value("parent");
		request.attr["width"] = e.value("width");
		request.attr["height"] = e.value("height");
		request.attr["orientation"] = e.value("orientation");
		request.attr["sub-type"] = e.value("sub-type");
		request.attr["checked"] = e.value("checked");
		request.attr["columns"] = e.value("columns");
		break;
	    }
	    case AlteratorRequestClose:
	    {
		request.attr["widget-id"] = e.value("widget-id");
		break;
	    }
	    case AlteratorRequestClean:
	    {
		request.attr["widget-id"] = e.value("widget-id");
		break;
	    }
	    case AlteratorRequestSet:
	    {
		request.attr["widget-id"] = e.value("widget-id");
		request.attr["name"] = e.value("name");
		request.attr["value"] = cmd->value_;
		break;
	    }
	    case AlteratorRequestEvent:
	    {
		request.attr["widget-id"] = e.value("widget-id");
		request.attr["value"] = cmd->value_;
		break;
	    }
	    case AlteratorRequestSplash:
	    {
		request.attr["value"] = cmd->value_;
		break;
	    }
	    case AlteratorRequestStart:
	    {
		request.attr["widget-id"] = e.value("widget-id");
		break;
	    }
	    case AlteratorRequestStop:
	    {
		request.attr["widget-id"] = e.value("widget-id");
		break;
	    }
	    case AlteratorRequestMessage:
	    {
		request.attr["type"] = e.value("type");
		request.attr["title"] = e.value("title");
		request.attr["message"] = e.value("message");
		request.attr["buttons"] = e.value("buttons");
		break;
	    }
	    case AlteratorRequestLanguage:
	    {
		request.attr["value"] = e.value(cmd->value_);
		break;
	    }
	    case AlteratorRequestRetry:
	    {
		break;
	    }
	    case AlteratorRequestCnstrAdd:
	    {
		request.attr["name"] = e.value("name");
		request.attr["type"] = e.value("type");
		request.attr["params"] = e.value("params");
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
/*
	    default:
	    {
		qDebug("Unknown alterator request action \"%s\".", qPrintable(e.value("action")));
		return;
	    }
*/
	}
	emit alteratorRequest(request);
}
