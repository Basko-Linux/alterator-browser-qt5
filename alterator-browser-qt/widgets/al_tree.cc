
#include <QHeaderView>

#include "al_tree.hh"
#include "a_pixmaps.hh"

ATreeWidget::ATreeWidget(QWidget *parent, const Qt::Orientation):
    AWidget<QTreeWidget>(parent)
{
}

ATreeWidget::~ATreeWidget()
{}

// alTree
alTree::alTree(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
	alWidgetPre<ATreeWidget>(attr,WTree,id,parent)
{
    expanded_ = false;
    //setings to be compatible with QListView
    wnd_->setUniformRowHeights(true);
    wnd_->setAllColumnsShowFocus(true);
    wnd_->setItemsExpandable(true);
    wnd_->setSelectionBehavior(QAbstractItemView::SelectRows);
    int col_count = 1;
    if( attr.contains(QStringLiteral("columns")) ) col_count = attr.value(QStringLiteral("columns")).i;
    if( col_count <1 ) col_count = 1;
    if( col_count > 1 )
	wnd_->setAlternatingRowColors(true);
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
	if (QStringLiteral("tree-items") == name)
	{
                wnd_->clear();//clear all previous content
		items_ = value;
		if (!coords_.isEmpty())
		{
		    setItems();
		    coordmap_ = coords_.split(QLatin1Char(';'));//move to internal storage
		    items_ = coords_ = QString();
		}
	}
	else if (QStringLiteral("coordinates") == name)
	{
		coords_ = value;
		if (!items_.isEmpty())
		{
		    setItems();
		    coordmap_ = coords_.split(QLatin1Char(';'));//move to internal storage
		    items_ = coords_ = QString();
		}
		expandOrCollapseAllTree();
		adjustAllColumnsWidth();
	}
	else if (QStringLiteral("current") == name)
	{
		QTreeWidgetItem *item = 0;
		QStringList coords = value.split(QLatin1Char(';'));
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
	else if (QStringLiteral("header") == name) 
	{
	    if( value.isEmpty() )
		wnd_->header()->hide(); 
	    else
		wnd_->header()->show(); 
	    wnd_->setHeaderLabels(value.split(QLatin1Char(';')));
	}
	else if (QStringLiteral("expanded") == name) 
	{
	    expanded_ = value == QStringLiteral("true");
	    expandOrCollapseAllTree();
	} 
	else
		alWidget::setAttr(name,value);
}

void alTree::registerEvent(const QString& name)
{
	if (QStringLiteral("selected") == name)
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
        data.replace(QLatin1Char(','), QLatin1Char(' '));
	return QStringLiteral("(current .(%1))").arg(data);
    }
    return QString();
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
	QStringList itemlist = items_.split(QLatin1Char(';'));
	QStringList coordlist = coords_.split(QLatin1Char(';'));
	const int len = itemlist.size();
	const int columns = wnd_->columnCount();
	for (int i=0;i+1<len;)
	{
		QStringList coords = coordlist[i/(2*columns)].split(QLatin1Char(','));
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
