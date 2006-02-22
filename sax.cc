#include <QTextStream>
#include <QXmlInputSource>
#include <QXmlSimpleReader>

#include "sax.hh"

//default Qt's QXmlInputSource uses readAll()
class alInputSource: public QXmlInputSource
{
	int pos_;
	QString buf_;
	QTextStream in_;
	
public:
	alInputSource():
		pos_(0),
		in_(stdin)
	{
		in_.setCodec("UTF-8");
	}

	void setData(const QString& dat) { buf_ = dat; }
        void setData(const QByteArray& dat) { buf_ = dat; }
        QString data() const { return buf_; }
        void reset() { pos_ = 0; }
        void fetchData()
	{
		buf_= in_.readLine();
		pos_ = 0;
	}
        virtual QChar next()
	{
		if (pos_ == buf_.length()) fetchData();
		return buf_[pos_++];
	}
	
};

//sax input handler
class alHandler : public QXmlDefaultHandler
{
    alCommand *command_;
public:
    alHandler(): command_(0),request_(new alRequest()) {}
    bool startElement(const QString &namespaceURI, const QString &localName,
                      const QString &qName, const QXmlAttributes &attributes);
    bool endElement(const QString &namespaceURI, const QString &localName,
                    const QString &qName);
    bool characters(const QString &str);
    alRequest *request_;
};



bool alHandler::startElement(const QString &namespaceURI, const QString &localName,
                             const QString &qName, const QXmlAttributes &attributes)
{
	if (qName == "auth-answer")
	{
		request_->attrs_ = attributes;
		return true;
	}
	else if (qName == "command")
	{
		command_ = new alCommand;
		command_->attrs_ = attributes;
		return true;
	}
	return false;
}

bool alHandler::endElement(const QString &namespaceURI, const QString &localName,
                           const QString &qName)
{
	if (qName == "auth-answer")
		return false;
	else if (qName == "command")
	{
		request_->commands_.append(command_);
		command_ = 0;
		return true;
	}
	else return false;
}

bool alHandler::characters(const QString &str)
{
	if (command_) command_->value_ = str;
	return true;
}



alRequest *readRequest()
{
	QXmlSimpleReader reader;
	alHandler handler;
	alInputSource source;
	reader.setContentHandler(&handler);
	reader.parse(&source);

	return handler.request_;
}



#if 0
int main()
{
	  QXmlSimpleReader reader;
	  alHandler handler;
	  alInputSource source;
	  reader.setContentHandler(&handler);
	  reader.parse(&source);

	  std::cerr<<"number of commands:"<<handler.request_->attrs_.count()<<std::endl;
}

#endif
