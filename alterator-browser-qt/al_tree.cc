
#include <QHeaderView>

#include "al_tree.hh"
#include "a_pixmaps.hh"

alTree::alTree(const QString& id,const QString& parent,const QString& columns):
	alWidgetPre<QTreeWidget>(Tree,id,parent)
{
    //setings to be compatible with QListView
    wnd_->setColumnCount(columns.isEmpty()? 1 : columns.toInt());
    wnd_->header()->hide();
    wnd_->setSelectionBehavior(QAbstractItemView::SelectRows);
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
	}
	if ("current" == name)
	{
		QStringList coords = value.split(";");
		QTreeWidgetItem *item = findPosition(wnd_->topLevelItem(coords[0].toInt()),
			                                                coords.mid(1),0);
		wnd_->scrollToItem(item);
		wnd_->setCurrentItem(item);
	}
	else if ("header" == name) 
	{ 
		wnd_->header()->show(); 
        	wnd_->setHeaderLabels(value.split(";")); 
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
	return QString("(current .")+"("+data+"))";
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

