#include "widgets.hh"
#include "constraints.hh"

namespace {

	bool needExclude(const rules& lst)
	{
		QMapIterator<QString,QString> it(lst);
		while(it.hasNext())
		{
			it.next();
			alWidget *excluder = findAlWidget(it.key());
			if (excluder && excluder->getValue() == it.value())
				return true;
		}
		return false;
	}

}


Constraints::Constraints():
	QObject()
{
}

void Constraints::clear()
{
	//unbind from signals
	QSetIterator<QString> it(excluders);
	while(it.hasNext())
	{
		alWidget *w = findAlWidget(it.next());
		if (w) QObject::disconnect(w,SIGNAL(updated()),this,SLOT(exclude()));
	}

	excluders.clear();
	excludings.clear();


	//clear marks on required fields
	QListIterator<QString> it1(required);
	while(it1.hasNext())
	{
		alWidget *w = findAlWidget(it1.next());
		if (w) w->markRequired(false);
	}
	required.clear();
}

void  Constraints::apply()
{
	//bind to signals
	QSetIterator<QString> it(excluders);
	while(it.hasNext())
	{
		alWidget *w = findAlWidget(it.next());
		if (w) QObject::connect(w,SIGNAL(updated()),this,SLOT(exclude()));
	}
	//recheck excludes now
	exclude();
	
	//mark required fields
	QListIterator<QString> it1(required);
	while(it1.hasNext())
	{
		alWidget *w = findAlWidget(it1.next());
		if (w) w->markRequired(true);
	}
}

void Constraints::add(const QString& name,const QString& type,const QString& params)
{
	if ("exclude" == type)
	{	
		QStringList data = params.split(";");
		if (data.size() > 1)
		{
			const QString& value = data[0];
			const QString& item = data[1];
			excludings[item][name]=value;
			excluders.insert(name);
		}
	}
	else if ("required" == type)
		required.push_back(name);
}

void Constraints::exclude()
{
	QMapIterator<QString,rules> it(excludings);
	while(it.hasNext())
	{
		it.next();
		alWidget *w = findAlWidget(it.key());
		if (w) w->getWidget()->setEnabled(!needExclude(it.value()));
	}
}
