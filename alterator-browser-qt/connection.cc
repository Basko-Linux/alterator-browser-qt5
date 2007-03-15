#include <string>
#include <iostream>

#include <QFile>
#include <QTextStream>
#include <QStringList>

#include "connection.hh"
#include "global.hh"

#include "main_window.hh"
extern MainWindow *main_window;

Connection::Connection(QObject *parent):
    QThread(parent)
{
    destruction = false;
    islong_timer = new QTimer(this);
    connect(this, SIGNAL(started()), this, SLOT(startDelayedFinish()));
    connect(this, SIGNAL(finished()), this, SLOT(endDelayedFinish()));
}

Connection::~Connection()
{
    destruction = true;
    disconnect();
    islong_timer->stop();
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
    if( !islong_timer->isActive() )
	islong_timer->singleShot(500, this, SLOT(checkRequestIsLong()));
}

void Connection::checkRequestIsLong()
{
    if( isRunning() )
    {
#if 1
	emit startLongRequest();
#else
	QEvent *e = new QEvent((QEvent::Type)EVENT_REQUEST_LONG_BEGIN);
	QCoreApplication::postEvent(main_window, e);
#endif
    }
}

void Connection::endDelayedFinish()
{
#if 1
    emit stopLongRequest();
#else
    QEvent *e = new QEvent((QEvent::Type)EVENT_REQUEST_LONG_END);
    QCoreApplication::postEvent(main_window, e);
#endif
}

void Connection::getDocParser(alCommand *cmd)
{
	QXmlAttributes e = cmd->attrs_;
	QString action = e.value("action");

	if ("new" == action)
		emit newRequest(e);
	else if ("close" == action)
		emit closeRequest(e.value("widget-id"));
	else if ("clean" == action)
		emit cleanRequest(e.value("widget-id"));
	else if ("set" == action)
		emit setRequest(e.value("widget-id"),
			   e.value("name"),
			   cmd->value_);
	else if ("create-event" == action)
		emit eventRequest(e.value("widget-id"), cmd->value_);
	else if ("splash" == action)
		emit splashMessageRequest(cmd->value_);
	else if ("start" == action)
		emit startRequest(e.value("widget-id"));
	else if ("stop" == action)
		emit stopRequest(e.value("widget-id"));
	else if ("messagebox" == action)
		emit messageboxRequest(e);
	else if ("language" == action)
		emit changeLanguageRequest(cmd->value_);
	else if ("retry" == action)
	{
	    emit retryRequest();
	}
	else if ("constraints-clear" == action)
	{
	    emit constraintsClearRequest();
	}
	else if ("constraints-apply" == action)
	{
	    emit constraintsApplyRequest();
	}
	else if ("constraints-add" == action)
	{
	    emit constraintsAddRequest(e.value("name"),e.value("type"),e.value("params"));
	}
	//qDebug("getDocParser action %s", action.toLatin1().data());
}
