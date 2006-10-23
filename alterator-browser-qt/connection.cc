#include <string>
#include <iostream>

#include <QFile>
#include <QTextStream>
#include <QStringList>

#include "connection.hh"

QString sessionId;
QString userId;

static
void parseAnswer(alRequest *dom,void(parser)(alCommand*))
{
	QListIterator<alCommand*> it(dom->commands_);
	while(it.hasNext())
	    parser(it.next());
}

QStringList languageList(const char *env)
{
	QStringList in = QString(env).split(":", QString::SkipEmptyParts);
	QStringList out;
	QStringListIterator it(in);
	while(it.hasNext())
	{
		QString item = it.next();
		if (!item.isEmpty()) out += item.replace(QRegExp("\\..*"),"");
	}
	return out;
}

static
QString createLangList()
{
	const char *env = getenv("LANGUAGE");
	if (env && *env) return languageList(env).join(";");
	env = getenv("LC_ALL");
	if (env && *env) return QString(env).replace(QRegExp("\\..*"),"");
	env = getenv("LC_MESSAGES");
	if (env && *env) return QString(env).replace(QRegExp("\\..*"),"");
	env = getenv("LANG");
	if (env && *env) return QString(env).replace(QRegExp("\\..*"),"");
	return "";
}


void initConnection(void (parser)(alCommand*))
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
	QString out;
	QTextStream s(&out);
	s<<"(auth-request user \""<<userId<<"\" session-id "
	 <<"\""<<sessionId<<"\""
	 <<" content "<<content<<")";
       return out;
}

void getDocument( void (parser)(alCommand*), const QString& content )
{
	std::cout<<makeRequest(content).toUtf8().data()
		 <<std::endl;
	std::auto_ptr<alRequest> dom(readRequest());

	parseAnswer(dom.get(),parser);
}
