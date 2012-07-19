
#include <QAbstractButton>

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

void WidgetList::removeFromListById(const QString& id)
{
    elements.remove(id);
}

void WidgetList::destroyLater(const QString& id)
{
    if( contains(id) )
    {
	alWidget *dead = elements.take(id);
	if( dead )
	{
	    groupRemove(dead);
	    deleteChildrenById(dead->getId());
	    dead->deleteLater();
	}
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

void WidgetList::groupAdd(alWidget *aw)
{
    if( aw )
    {
	QString altgroup = aw->getGroup();
	if( !altgroup.isEmpty() )
	{
	    groups.insert(altgroup, aw);
	    //qDebug("Add widget<%s> to group<%s>", qPrintable(aw->getId()), qPrintable(altgroup));
	    switch( aw->type() ) {
		case WRadio:
		{
		    if( !buttongroups.contains( altgroup ) )
		    {
			QButtonGroup *bgrp = new QButtonGroup(this);
			buttongroups[altgroup] = bgrp;
		    }
		    QAbstractButton *ab = qobject_cast<QAbstractButton*>(aw->getWidget());
		    if( ab )
		    {
			//ab->setAutoExclusive(false);
			buttongroups[altgroup]->addButton(ab);
		    }
		    else
			qWarning("Widget is not an QAbstractButton");
		    break;
		}
		default:
		    break;
	    }
	}
    }
}

void WidgetList::groupRemove(alWidget *aw)
{
    if( aw )
    {
	QString altgroup = aw->getGroup();
	if( !altgroup.isEmpty() )
	{
	    if( groups.contains(altgroup, aw) )
	    {
		groups.remove(altgroup, aw);
		//qDebug("Remove widget<%s> from group <%s>", qPrintable(aw->getId()), qPrintable(altgroup));
		switch( aw->type() )
		{
		    case WRadio:
		    {
			if( buttongroups.contains(altgroup) )
			{
			    QButtonGroup *bgrp = buttongroups[altgroup];
			    QWidget *w = aw->getWidget();
			    if( w )
			    {
				QAbstractButton *ab = qobject_cast<QAbstractButton*>(w);
				if( ab )
				    bgrp->removeButton(ab);
				if( bgrp->buttons().size() <= 0 )
				    buttongroups.remove(altgroup);
			    }
			}
			break;
		    }
		    default:
			break;
		}
	    }
	}
    }
}

void WidgetList::deleteChildrenById(const QString& id)
{
    if(!id.isEmpty())
    {
	Q_FOREACH(alWidget *child, alChildrenById(id))
	{
	    child->show(false);
	    destroyLater(child->getId());
	}
    }
}

void WidgetList::destroyAll()
{
    QMapIterator<QString,alWidget*> i(elements);
    while(i.hasNext())
    {
	i.next();
	destroyLater(i.key());
    }
}
