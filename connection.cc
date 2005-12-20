#include <string>
#include <iostream>

#include <QFile>
#include <QTextStream>
#include <QStringList>

#include "connection.hh"

#include <iostream>

QString sessionId;

//Qt suxx to made such work well, so I use STL here
static
std::string readChunk()
{
	std::string str;
	std::string result;
	while(std::getline(std::cin,str) && (str != "--stop--"))
	     result += str + "\n";
	return result;
}

static
QDomDocument *readAnswer()
{
	QDomDocument *dom = new QDomDocument();
	QString error;
	int line;
	int column;
	
	if (!dom->setContent(QString::fromUtf8(readChunk().c_str()),
	                    false,
			    &error,
			    &line,
			    &column))
		{
			std::cerr<<"xml parsing error:"<<error.toLatin1().data()
			         <<"line:"<<line<<"column:"<<column<<std::endl;
			exit(1);
		}
	return dom;
}


static
void parseAnswer(QDomDocument *dom,void(parser)(const QDomElement&))
{
	//process answer
	QDomNodeList lst = dom->elementsByTagName("command");
	int num = lst.length();
	for(int i=0;i<num;++i)
	{
		QDomNode o = lst.item(i);
		if (!o.isElement()) continue;
		parser(o.toElement());
	}
}

QStringList languageList(const char *env)
{
	QStringList in = QString(env).split(":");
	QStringList out;
	QStringListIterator it(in);
	while(it.hasNext())
	{
		QString item = it.next();
		if (!item.isEmpty()) out += item.left(2);
	}
	return out;
}

static
QString createLangList()
{
	QStringList lst;
	const char *env = getenv("LANGUAGE");
	if (env && *env) lst += languageList(env);
	env = getenv("LC_ALL");
	if (env && *env) lst += QString(env).left(2);
	env = getenv("LC_MESSAGES");
	if (env && *env) lst += QString(env).left(2);
	env = getenv("LANG");
	if (env && *env) lst += QString(env).left(2);
	return lst.join(";");
}


void initConnection(void (parser)(const QDomElement&))
{
	std::cout<<"(auth-request user \"qtbrowser\" password \"\" "
	<<"language \""<<createLangList().toLatin1().data()<<"\""
	<<")"<<std::endl;
	std::auto_ptr<QDomDocument> dom(readAnswer());
	
	QDomNodeList lst = dom->elementsByTagName("auth-answer");
	if (!lst.length() ||
	    !lst.item(0).isElement() ||
	    !lst.item(0).toElement().hasAttribute("session-id"))
	{
		qDebug("wrong answer type");
		exit(1);
	}
	sessionId = lst.item(0).toElement().attribute("session-id");
	
	parseAnswer(dom.get(),parser);
}

QString makeRequest(const QString& content)
{
	QString out;
	QTextStream s(&out);
	s<<"(auth-request user \"qtbrowser\" session-id "
	 <<"\""<<sessionId<<"\""
	 <<" content "<<content<<")";
       return out;
}

void getDocument( void (parser)(const QDomElement&), const QString& content )
{
	std::cout<<makeRequest(content).toUtf8().data()
		 <<std::endl;
	std::auto_ptr<QDomDocument> dom(readAnswer());

	parseAnswer(dom.get(),parser);
}
