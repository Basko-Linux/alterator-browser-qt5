#include <string>
#include <iostream>

#include <QFile>
#include <QTextStream>

#include "connection.hh"

#include <iostream>

QString sessionId;

//Qt suxx to made such work well, so I use STL here
std::string readChunk()
{
	std::string str;
	std::string result;
	while(std::getline(std::cin,str) && (str != "--stop--"))
	     result += str + "\n";
	return result;
}

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


void initConnection()
{
	std::cout<<"(auth-request user \"qtbrowser\" password \"\")"<<std::endl;
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
	QDomNodeList lst = dom->elementsByTagName("command");
	int num = lst.length();
	for(int i=0;i<num;++i)
	{
		QDomNode o = lst.item(i);
		if (!o.isElement()) continue;
		parser(o.toElement());
	}
}
