#include <string>
#include <iostream>

#include <QFile>
#include <QTextStream>
#include <QStringList>

#include "connection.hh"
#include "main_window.hh"

extern MainWindow *main_window;
QString sessionId;
QString userId;

static
void parseAnswer(alRequest *dom,parserfunc parser)
{
    if( main_window ) main_window->setCursor(Qt::ArrowCursor);
    QListIterator<alCommand*> it(dom->commands_);
    while(it.hasNext())
    {
	parser(it.next());
    }
}

static
QString createLangList()
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


void initConnection(parserfunc parser)
{
	std::cout<<"(auth-request user \"qtbrowser\" password \"\" "
	<<"language \""<<createLangList().toLatin1().data()<<"\""
	<<")"<<std::endl;
	std::auto_ptr<alRequest> dom(readRequest());
	
	sessionId = dom->attrs_.value("session-id");
	userId = dom->attrs_.value("user");
//	std::cerr<<"session-id="<<sessionId.toLatin1().data()<<std::endl;
	
	parseAnswer(dom.get(),parser);
}

QString makeRequest(const QString& content)
{
    if( main_window ) main_window->setCursor(Qt::BusyCursor);
	QString out;
	QTextStream s(&out);
	s<<"(auth-request user \""<<userId<<"\" session-id "
	 <<"\""<<sessionId<<"\""
	 <<" content "<<content<<")";
       return out;
}

void getDocument( parserfunc parser, const QString& content )
{
	std::cout<<makeRequest(content).toUtf8().data()
		 <<std::endl;
	std::auto_ptr<alRequest> dom(readRequest());

	parseAnswer(dom.get(),parser);
}
