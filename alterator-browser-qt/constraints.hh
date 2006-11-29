#ifndef QTBROWSER_CONSTRAINTS_HH
#define QTBROWSER_CONSTRAINTS_HH

#include <QObject>
#include <QMap>
#include <QSet>
#include <QList>
#include <QString>

//excluder+value
typedef QMap<QString,QString> rules;

class Constraints: public QObject
{
Q_OBJECT

	//excluded widgets and + its restrictions
	QMap<QString,rules> excludings;
	QSet<QString>	excluders;
	
	QList<QString> required;

	public:
		Constraints();
		virtual ~Constraints() {}
		void clear();
		void add(const QString& name,const QString& type,const QString& params);
		void apply();
	public slots:
		void exclude();
};

#endif
