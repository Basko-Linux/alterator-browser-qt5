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
    connect(islong_timer, SIGNAL(timeout()), this, SLOT(checkRequestIsLong()));
    connect(this, SIGNAL(finished()), this, SLOT(endDelayedFinish()));
}

Connection::~Connection()
{
    destruction = true;
    disconnect();
    islong_timer->disconnect();
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
    main_window->startBusy();
    wait();
    request_string = makeRequest(content);
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

QString Connection::createLangList()
{
    QString lang(getenv("LC_ALL"));
    QString language(getenv("LANGUAGE"));

    if( !language.isEmpty() )
	lang += ":" + language;
    if( lang.isEmpty() )
	lang = getenv("LC_MESSAGES");
    if( lang.isEmpty() )
	lang = getenv("LANG");
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
    std::cout<< request_string.toUtf8().data() << std::endl << std::flush;
    std::auto_ptr<alRequest> dom(readRequest());
    parseAnswer(dom.get());
}

void Connection::startDelayedFinish()
{
    if( !islong_timer->isActive() )
    {
	islong_timer->setInterval(500);
	islong_timer->start();
    }
}

void Connection::checkRequestIsLong()
{
    if( isRunning() )
    {
	islong_timer->stop();
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
    stopLongRequest();
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
