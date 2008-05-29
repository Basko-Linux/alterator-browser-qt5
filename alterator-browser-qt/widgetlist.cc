
#include "widgetlist.hh"

WidgetList::WidgetList(QObject* parent):
    QObject(parent)
{
}

WidgetList::~WidgetList()
{
}

QLayout* WidgetList::findViewLayout(const QString& id)
{
	QLayout *l = 0;
	alWidget *aw = alWidgetById(id);
	if( aw )
	    l = aw->getViewLayout();
	return l;
}

bool WidgetList::contains(const QString& id)
{
    return elements.contains(id);
}

alWidget* WidgetList::alWidgetById(const QString& id)
{
    if( contains(id) )
    {
	return elements[id];
    }
    else
    {
	return 0;
    }
}

QList<alWidget*> WidgetList::alChildrenById(const QString& id)
{
    QList<alWidget*> children;
    QMapIterator<QString,alWidget*> it(elements);
    while(it.hasNext())
    {
	it.next();
	alWidget *aw = it.value();
	if( aw->getParentId() == id )
	    children << aw;
    }
    return children;
}

alWidget* WidgetList::alWidgetByName(const QString& name)
{
    QMapIterator<QString,alWidget*> it(elements);
    while(it.hasNext())
    {
	it.next();
	alWidget *item = it.value();
	if (item->objectName() == name)
	{
	    return item;
	}
    }
    return 0;
}

QString WidgetList::makeRequestData()
{
    QString request;
    QMapIterator<QString,alWidget*> it(elements);
    while(it.hasNext())
    {
	it.next();
	QString data = it.value()->postData();
	if (!data.isEmpty())
		request += " \n("+it.key() + " "+data+ ")";
    }
    return request;
}

void WidgetList::destroyLater(const QString& id)
{
    if( contains(id) )
    {
	elements.take(id)->destroyLater();
    }
}

QWidget* WidgetList::qWidgetById(const QString& id)
{
    QWidget *w = 0;
    alWidget *aw = widgetlist->alWidgetById(id);
    if( aw )
	w = aw->getWidget();
    return w;
}

void WidgetList::add(const QString& id, alWidget* aw)
{
    if(aw)
	elements[id] = aw;
}

void WidgetList::removeFromListById(const QString& id)
{
    elements.remove(id);
}

void WidgetList::deleteChildrenById(const QString& id)
{
    if(!id.isEmpty())
    {
	foreach(alWidget *child, alChildrenById(id))
	{
	    delete child;
	}
    }
}
