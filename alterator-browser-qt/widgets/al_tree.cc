
#include <QHeaderView>

#include "al_tree.hh"
#include "a_pixmaps.hh"

alTree::alTree(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent,const QString& columns):
	alWidgetPre<QTreeWidget>(attr,WTree,id,parent)
{
    expanded_ = false;
    //setings to be compatible with QListView
    wnd_->setAlternatingRowColors(true);
    wnd_->setAllColumnsShowFocus(true);
    wnd_->setUniformRowHeights(true);
    wnd_->setSelectionBehavior(QAbstractItemView::SelectRows);
    int col_count = columns.toInt();
    if( col_count <1 ) col_count = 1;
    wnd_->setColumnCount(col_count);
    wnd_->header()->hide();
    wnd_->setSelectionBehavior(QAbstractItemView::SelectRows);
    connect(wnd_, SIGNAL(itemCollapsed(QTreeWidgetItem*)),
	    this, SLOT(adjustFirstColumnWidth(QTreeWidgetItem*)));
    connect(wnd_, SIGNAL(itemExpanded(QTreeWidgetItem*)),
	    this, SLOT(adjustFirstColumnWidth(QTreeWidgetItem*)));
}

void alTree::setAttr(const QString& name,const QString& value)
{
	if ("tree-items" == name)
	{
                wnd_->clear();//clear all previous content
		items_ = value;
		if (!coords_.isEmpty())
		{
		    setItems();
		    coordmap_ = coords_.split(";");//move to internal storage
		    items_ = coords_ = "";
		}
	}
	if ("coordinates" == name)
	{
		coords_ = value;
		if (!items_.isEmpty())
		{
		    setItems();
		    coordmap_ = coords_.split(";");//move to internal storage
		    items_ = coords_ = "";
		}
		expandOrCollapseAllTree();
		adjustAllColumnsWidth();
	}
	if ("current" == name)
	{
		QTreeWidgetItem *item = 0;
		QStringList coords = value.split(";");
		bool ok = false;
		int pos = coords[0].toInt(&ok);
		if( pos >= 0 && ok) 
		    item = findPosition(wnd_->topLevelItem(pos), coords.mid(1),0);

		if( item )
		{
		    wnd_->setCurrentItem(item);
		    wnd_->scrollToItem(item);
		}
		else
		{
		    QList<QTreeWidgetItem*> si = wnd_->selectedItems();
		    QListIterator<QTreeWidgetItem*> it(si);
		    while( it.hasNext() )
		    {
			QTreeWidgetItem *itm = it.next();
			itm->setSelected(false);
		    }
		    wnd_->setCurrentItem(0);
		}
	}
	else if ("header" == name) 
	{ 
		wnd_->header()->show(); 
        	wnd_->setHeaderLabels(value.split(";")); 
	} 
	else if ("expanded" == name) 
	{
	    expanded_ = value == "true";
	    expandOrCollapseAllTree();
	} 
	else
		alWidget::setAttr(name,value);
}

void alTree::registerEvent(const QString& name)
{
	if ("selected" == name)
		connect(wnd_,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
			     SLOT(onSelect(QTreeWidgetItem*,QTreeWidgetItem*)));
}

QString alTree::postData() const
{
    QTreeWidgetItem *i = wnd_->currentItem();
    if (i)
    {
	const int columns = wnd_->columnCount();
	QString data = coordmap_[i->type()/(2*columns)];
        data.replace(","," ");
	return QString("(current .(%1))").arg(data);
    }
    return "";
}

QTreeWidgetItem *alTree::findPosition(QTreeWidgetItem *parent,QStringList coords,int deep)
{
	if (!parent)
		return 0;
	else if (coords.size() == deep)
		return parent;
	else
		return findPosition(parent->child(coords[0].toInt()),coords.mid(1),deep);
}

void alTree::setItems()
{
	QStringList itemlist = items_.split(";");
	QStringList coordlist = coords_.split(";");
	const int len = itemlist.size();
	const int columns = wnd_->columnCount();
	for (int i=0;i+1<len;)
	{
		QStringList coords = coordlist[i/(2*columns)].split(",");
		const int len = coords.size();
		
		QTreeWidgetItem *item;
		if (len == 1)
			item = new QTreeWidgetItem(wnd_,i);
		else
			item = new QTreeWidgetItem(findPosition(wnd_->topLevelItem(coords[0].toInt()),
			                                                         coords.mid(1),1),
								    i);
		
		for(int col=0;col < columns; ++col,i+=2)
		{
			QString data = itemlist[i];
			QString pixmap = itemlist[i+1];
			item->setText(col,data);
			if (!pixmap.isEmpty())
                		item->setIcon(col,QIcon(getPixmap(pixmap)));
		}
	}
}

void alTree::adjustFirstColumnWidth(QTreeWidgetItem*)
{
    adjustFirstColumnWidth();
}

void alTree::adjustFirstColumnWidth()
{
    int n_columns = wnd_->columnCount();
    if( n_columns > 0 )
    {
	wnd_->resizeColumnToContents(0);
    }
}

void alTree::adjustAllColumnsWidth()
{
    int n_columns = wnd_->columnCount();
    if( n_columns > 0 )
    {
	for(int col=0;col < n_columns; col++)
	{
	    wnd_->resizeColumnToContents(col);
	}
    }
}

void alTree::expandOrCollapseAllTree()
{
    QTreeWidgetItemIterator it(wnd_);
    while (*it)
    {
	QTreeWidgetItem *item = *it;
	if( expanded_ )
	{
	    if( item->childCount() > 0 && !item->isExpanded() )
		item->setExpanded(true);
	}
	else
	{
	    if( item->childCount() > 0 && item->isExpanded() )
		item->setExpanded(false);
	}
	++it;
    }
}
