#include "global.hh"

#include "widgets.hh"
#include "a_pixmaps.hh"
#include "utils.hh"


void updateHelp(const QString& value)
{
	QMapIterator<QString,alWidget*> i(elements);
	while (i.hasNext())
	{
		i.next();
		if( (i.value())->type() == alWidget::HelpPlace )
		{
			i.value()->setAttr("source",value);
			break;
		}
	}	
}

QLayout *findViewLayout(const QString& id)
{
	if (!elements.contains(id)) return 0;
	alWidget *aw = elements[id];
	return aw->getViewLayout();
}

QWidget* findQWidget(const QString& id)
{
	if (!elements.contains(id)) return 0;
	QWidget *w = elements[id]->getWidget();
	if( w )	return w;
	return 0;
}

alWidget* findAlWidgetById(const QString& id)
{
	if (!elements.contains(id)) return 0;
	return elements[id];
}

alWidget *findAlWidgetByName(const QString& name)
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

QList<alWidget*> findAlChildren(const QString& id)
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

void collectTabIndex(QList<QString>& parents, QMap<QString, QMap<int,QWidget*> >& order, alWidget *wdg, const QString& s_index)
{
    bool ok = false;
    int tab_index = s_index.toInt(&ok);
    if( ok && wdg )
    {
        QString parent_id = wdg->getParentId();
        if( !parent_id.isEmpty() )
        {
	    if( !parents.contains(parent_id) )
		parents.append(parent_id);
	    order[parent_id].insert(tab_index, wdg->getWidget());
	}
    }
}
