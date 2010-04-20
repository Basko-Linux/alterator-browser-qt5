
#include <QHeaderView>

#include "al_checktree.hh"
#include "a_pixmaps.hh"
#include "browser.hh"

#define ACHECKTREE_ID_ROLE 1000

// Tree widget with checkboxes
// http://www.altlinux.org/Alterator/CheckTree

ACheckTree::ACheckTree(QWidget *parent, const Qt::Orientation):
    AWidget<QTreeWidget>(parent)
{
    header()->hide();
    setUniformRowHeights(true);
    setItemsExpandable(true);
    setAllColumnsShowFocus(true);
    setSortingEnabled(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
   
    // Connect to itemChanged signal
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem *, int)), this, SLOT(onStateChanged(QTreeWidgetItem *, int))) ;
    // Connect to selected signal
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelect())) ;
    
}

ACheckTree::~ACheckTree()
{
}

// Add single row. Tuple should contains: item_id, item_parent_id, label(s) for first and other columns
// If parent_name is empty string, tree item is top-level item.
void ACheckTree::addRow(const QStringList &data)
{
    int c = data.size();

    if (c < 3 ) 
	return;

    QString item_id;
    QString item_parent_id;

    QTreeWidgetItem *item = new QTreeWidgetItem();

    // Set default state
    item->setCheckState(0, Qt::Unchecked);

    // Set item_id and item_parent_id
    item_id = data.at(0);
    item_parent_id = data.at(1);

    // Fill second and other column fields
    for(int j=0;j < columnCount() && c > j+3;j++) 
    {
	item->setText(j, data.at(j+2));
    }

    // Set item id
    if( ! item_id.isEmpty() )
	item->setData(0, ACHECKTREE_ID_ROLE, QVariant(item_id));

    // Tie to parent
    QTreeWidgetItem *p = lookupItem(item_parent_id);
    if ( p ) {
	p->addChild(item);
    } else {
	if (item_parent_id.isEmpty())
	    addTopLevelItem(item);
	else {
	    orphaned.insert(item_parent_id, item);
	    //qDebug(qPrintable(QString("Parent '%1' not found.").arg(item_parent_id)));
	}
    }

    // Check for other orphan
    if ( ! item_id.isEmpty() && orphaned.count() > 0) {
	QList<QTreeWidgetItem*> items(orphaned.values(item_id));
	if( items.size() > 0 )
	{
	    foreach(QTreeWidgetItem *i, items)
		item->addChild(i);
	    orphaned.remove(item_id);
	}
    }

    // TODO Does not work with child items
    if (columnCount() > 1)
	resizeColumnToContents(0);
    item->setExpanded(false);
}

// Fill all items
void ACheckTree::setRows(const QStringList &data)
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
    QString item_id;

    // Iterate across all items to tree
    QTreeWidgetItemIterator it(this);
    while (*it) 
    {
	item_id = (*it)->data(0, ACHECKTREE_ID_ROLE).toString();
	if ((*it)->checkState(0) == Qt::Checked && ! item_id.isEmpty())
	{
	    selected.append(item_id);
	}
        ++it;
    }

    return selected;
}

QString ACheckTree::current()
{
    // Return current selected item
    QTreeWidgetItemIterator it(this);
    while (*it) 
    {
	if ((*it)->isSelected())
	    return (*it)->data(0, ACHECKTREE_ID_ROLE).toString();
	++it;
    }
    return QString();
}


// Slot for item state changed
void ACheckTree::onStateChanged(QTreeWidgetItem *item, int column)
{
    QString name;
    bool state;
    
    //qDebug("Item is changed");
    if( item && column == 0 ) {
	name  = item->data(0, ACHECKTREE_ID_ROLE).toString();
	state = item->checkState(0) == Qt::Checked;
	//qDebug(qPrintable(QString("%1: %2").arg(name).arg((state ? "checked" : "unchecked"))));
	if( eventRegistered(BrowserEventChanged) )
	    browser->emitEvent(getId(), BrowserEventChanged, AlteratorRequestDefault);
    }
}

// Slot for item select
void ACheckTree::onSelect()
{
    if( eventRegistered(BrowserEventChanged) )
	browser->emitEvent(getId(), BrowserEventSelected, AlteratorRequestDefault);
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
QTreeWidgetItem *ACheckTree::lookupItem(const QString& item_id)
{
    QString id;
    
    if (item_id.isEmpty())
	return NULL;
    
    // Iterate across all items to tree
    QTreeWidgetItemIterator it(this);
    while (*it) 
    {
	id = (*it)->data(0, ACHECKTREE_ID_ROLE).toString();
	if (item_id == id)
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
    int col_count = 1;
    if( attr.contains("columns") ) col_count = attr.value("columns").i;
    wnd_->setColumnCount(col_count);
    if( col_count > 1 ) 
	wnd_->setAlternatingRowColors(true);
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
	else if ("expand-rows" == name)
	{
	    QStringList data = value.split(";");
	    foreach(QString id, data)
	    {
		QTreeWidgetItem* item = wnd_->lookupItem(id);
		if( item )
		    item->setExpanded(true);
	    }
	}
	else if ("collapse-rows" == name)
	{
	    QStringList data = value.split(";");
	    foreach(QString id, data)
	    {
		QTreeWidgetItem* item = wnd_->lookupItem(id);
		if( item )
		    item->setExpanded(false);
	    }
	}
	else if ("current-rows" == name)
	{
	    QStringList data = value.split(";");
	    foreach(QString id, data)
	    {
		QTreeWidgetItem* item = wnd_->lookupItem(id);
		if( item )
		    item->setCheckState(0, Qt::Checked);
	    }
	}
	else if ("current" == name)
	{
	    // Set item selected
	    QTreeWidgetItem* item = wnd_->lookupItem(value);
	    if( item )
		item->setSelected(true);
	}
	else if ("icon-rows" == name)
	{
	    QString pixname;
	    QStringList data = value.split(";");
	    
	    QIcon pixmap = getPixmap(data.at(0).isEmpty()? "theme:null": data.at(0));
	    data.removeAt(0);
	    
	    foreach(QString id, data)
	    {
		QTreeWidgetItem* item = wnd_->lookupItem(id);
		
		if( item )
		    item->setIcon(0, pixmap);
	    }
	}
	else
	    alWidget::setAttr(name,value);
}

void alCheckTree::registerEvent(const QString& name)
{
	if ("selected" == name)
		wnd_->setEventRegistered(id_, BrowserEventSelected);
	else if ("changed" == name)
		wnd_->setEventRegistered(id_, BrowserEventChanged);
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
    QString selected_item_id;
    
    // Current element id
    ret.append(QString(" (current . %1)").arg(wnd_->current()));
        
    // Current checked elements
    ret.append(" (current-rows . (");
    foreach(QString item, wnd_->getSelected())
    {
	ret.append(QString(" %1").arg(item));
    }
    ret.append("))");
    
    
    
    return ret;
}

