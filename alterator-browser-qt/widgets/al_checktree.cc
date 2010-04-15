
#include <QHeaderView>

#include "al_checktree.hh"

/* TODO:
    + delayed parent bind
    + state changed signal
*/

ACheckTree::ACheckTree(QWidget *parent, const Qt::Orientation):
    AWidget<QTreeWidget>(parent)
{
    header()->hide();
    setUniformRowHeights(true);
    setItemsExpandable(true);
    setAllColumnsShowFocus(true);
    setSortingEnabled(false);
    
    // Connect to itemChanged signal
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(onStateChanged(QTreeWidgetItem *, int))) ;
    
}

ACheckTree::~ACheckTree()
{
}

// Add single row. Tuple should contains: checked_state, label, item_name, parent_name, expanded
// If item_name or parent_name is empty string, tree item is top-level item.
void ACheckTree::addRow(QStringList data)
{
    int c = data.size();
    
    if (c < 2 ) 
	return;

    QString item_checked;
    QString item_label;
    QString item_name;
    QString item_parent;
    QString item_expanded;
    
    item_checked = data.at(0);
    item_label = data.at(1);

    QTreeWidgetItem *item = new QTreeWidgetItem();

    // Set state and label
    if (item_checked == "true")
	item->setCheckState(0, Qt::Checked);
    else
	item->setCheckState(0, Qt::Unchecked);

    item->setText(0, item_label);
   
    // Set item_name and item_parent
    if (c > 2) 
	item_name = data.at(2);
    if (c > 3) 
	item_parent = data.at(3);
    if (c > 4) 
	item_expanded = data.at(4);

    // Set item id
    if( ! item_name.isEmpty() )
	item->setData(0, 1000, QVariant(item_name));

    // Tie to parent
    QTreeWidgetItem *p = lookupItem(item_parent);
    if ( p ) {
	p->addChild(item);
    } else {
	if (item_parent.isEmpty())
	    addTopLevelItem(item);
	else {
	    OrphanedTreeItem orphan;
	    orphan.parent = item_parent;
	    orphan.item = item;
	    orphaned.append(orphan);
	    //qDebug(qPrintable(QString("Parent '%1' not found.").arg(item_parent)));
	}
    }

    // Check for other orphan
    if ( ! item_name.isEmpty() && orphaned.count() > 0) {
	QListIterator<OrphanedTreeItem> i(orphaned);
	int pos=0;
	while (i.hasNext()) {
	    OrphanedTreeItem orphan = i.next();
	    if (orphan.parent == item_name) {
		item->addChild(orphan.item);
		orphaned.removeAt(pos);
	    }
	    pos++;
	}
    }

    // Set expanded state
    if (item_expanded == "false")
	item->setExpanded(false);
    else
	item->setExpanded(true);

    
}

// Fill all items
void ACheckTree::setRows(QStringList data)
{
    clear();
    const int columns = columnCount();
    QStringListIterator it(data);
    while( it.hasNext() )
    {
	QStringList row_data;
	while( it.hasNext() )
	{
	    row_data.append(it.next());
	    if( it.hasNext() )
		row_data.append(it.next());
	    if( row_data.size() >= columns+1 )
	    {
		addRow(row_data);
		break;
	    }
	}
    }
}

QStringList ACheckTree::getSelected() 
{
    QStringList selected;
    QString iName;

    // Iterate across all items to tree
    QTreeWidgetItemIterator it(this);
    while (*it) 
    {
	iName = (*it)->data(0, 1000).toString();
	if ((*it)->checkState(0) == Qt::Checked && ! iName.isEmpty())
	{
	    selected.append(iName);
	}
        ++it;
    }

    return selected;
}

// Slot for item state changed
void ACheckTree::onStateChanged(QTreeWidgetItem *item, int column)
{
    QString name;
    bool state;
    
    //qDebug("Item is changed");
    if( item && column == 0 ) {
	name  = item->data(0, 1000).toString();
	state = item->checkState(0) == Qt::Checked;
	//qDebug(qPrintable(QString("%1: %2").arg(name).arg((state ? "checked" : "unchecked"))));
    }
}

// Toggle checking via Space key pressed on current item
void ACheckTree::keyPressEvent(QKeyEvent * e) 
{
    if ((e->key() == Qt::Key_Space))
    {
	QTreeWidgetItem *ci = currentItem();
	if ( ci ) {
	    if (ci->checkState(0) == Qt::Unchecked)
		ci->setCheckState(0, Qt::Checked);
	    else
		ci->setCheckState(0, Qt::Unchecked);
	    ci->setSelected(true);
	}
        //emit spaceBtnPressed();
    }
    else
	QTreeWidget::keyPressEvent(e);
}

void ACheckTree::showEvent(QShowEvent*)
{
    //QTreeWidget::showEvent(e);
    scrollTo(currentIndex());
}

// Lookup item by its name
QTreeWidgetItem *ACheckTree::lookupItem(const QString& name)
{
    QString iName;
    
    if (name.isEmpty())
	return NULL;
    
    // Iterate across all items to tree
    QTreeWidgetItemIterator it(this);
    while (*it) 
    {
	iName = (*it)->data(0, 1000).toString();
	if (name == iName)
	{
	    return (*it);
	}
         ++it;
    }
    return NULL;
}

// alCheckTree

alCheckTree::alCheckTree(const AlteratorWidgetType awtype, const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
	alWidgetPre<ACheckTree>(attr,awtype,id,parent)
{
}

void alCheckTree::setAttr(const QString& name,const QString& value)
{
	if ("append-row" == name)
	{
	    QStringList data(value.split(";", QString::KeepEmptyParts));
	    wnd_->addRow(data);
	}
	else if ("rows" == name)
	{
	    QStringList data(value.split(";", QString::KeepEmptyParts));
	    wnd_->setRows(data);
	}
	else if ("rows-clear" == name)
	{
	    wnd_->clear();
	}
	else
	    alWidget::setAttr(name,value);
}

void alCheckTree::registerEvent(const QString& name)
{
	if ("selected" == name)
		connect(wnd_,SIGNAL(selected()),SLOT(onSelect()));
	else if ("changed" == name)
		connect(wnd_,SIGNAL(itemChanged()),SLOT(onChange()));
	else if ("clicked" == name)
	{
		connect(wnd_,SIGNAL(itemPressed(QTreeWidgetItem*,int)), SLOT(onClick(QTreeWidgetItem*,int)));
		connect(wnd_,SIGNAL(spaceBtnPressed()), SLOT(onClick()));
	}
	else if ("return-pressed" == name)
		connect( wnd_, SIGNAL(itemActivated(QTreeWidgetItem*,int)), SLOT(onReturn(QTreeWidgetItem*,int)));
	else if ("double-clicked" == name)
		connect(wnd_,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(onDoubleClick(QTreeWidgetItem*,int)));
}

QString alCheckTree::postData() const
{
    QString ret;
    
    ret.append(" ( groups . (");
    foreach(QString item, wnd_->getSelected())
    {
	ret.append("\"");
	ret.append(item);
	ret.append("\" ");
    }
    ret.chop(1);
    ret.append("))");
    return ret;
}

