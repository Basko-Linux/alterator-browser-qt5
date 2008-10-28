#ifndef QT_BROWSER_SAX_HH
#define QT_BROWSER_SAX_HH

#include <QList>
#include <QXmlAttributes>

//input command
struct alCommand
{
	QString value_;
	QXmlAttributes attrs_;
};

//input request
struct alRequest
{
	~alRequest()
	{
		qDeleteAll(commands_.begin(), commands_.end());
      		commands_.clear();
	}

	QList<alCommand*> commands_;
	QXmlAttributes attrs_;
};

alRequest readRequest();

#endif
