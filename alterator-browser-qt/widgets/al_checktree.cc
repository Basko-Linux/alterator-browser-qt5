
#include <QHeaderView>

#include "al_checktree.hh"
#include "a_pixmaps.hh"
#include "browser.hh"

#define ACHECKTREE_ID_ROLE 1000
#define ACHECKTREE_ROW_OPTS 2

// Tree widget with checkboxes
// http://www.altlinux.org/Alterator/CheckTree

ACheckTree::ACheckTree(QWidget *parent, const Qt::Orientation):
    AWidget<QTreeWidget>(parent)
{
    deferred_change = false;

    header()->hide();
    setUniformRowHeights(true);
    setItemsExpandable(true);
    setAllColumnsShowFocus(true);
    setSortingEnabled(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    // Connect to changed signal
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(onChange(QTreeWidgetItem*, int)));
    // Connect to expand signal
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(onExpand(QTreeWidgetItem *)));
    
    // Set initial states
    processed_parents = false;
    processed_children = false;
}

ACheckTree::~ACheckTree()
{
}

// Add single row. Tuple should contains: item_id, item_parent_id, label(s) for first and other columns
// If parent_name is empty string, tree item is top-level item.
void ACheckTree::addRow(const QStringList &data)
{
    if(data.size() < ACHECKTREE_ROW_OPTS + 1) return;

    QStringListIterator it(data);
    QTreeWidgetItem *item = new QTreeWidgetItem();

    // Set item id
    QString item_id(it.next());
    if( !item_id.isEmpty() )
	item->setData(0, ACHECKTREE_ID_ROLE, QVariant(item_id));

    // Set default state
    item->setFlags(item->flags() | Qt::ItemIsTristate);
    item->setCheckState(0, Qt::Unchecked);
    item->setExpanded(false);

    QString item_parent_id(it.next());

    int i = 0;
    while(it.hasNext())
    {
	item->setText(i, it.next());
	i++;
    }

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

    if (columnCount() > 1)
    {
	for (int cc=0; cc<columnCount(); cc++)
	    resizeColumnToContents(cc);
    }
}

// Fill all items
void ACheckTree::setRows(const QStringList &data)
{
    orphaned.clear();
    const int row_count = ACHECKTREE_ROW_OPTS + columnCount();
    for(int i=0; i < data.size(); i+=row_count)
    {
	addRow(data.mid(i,row_count));
    }
    orphaned.clear();
}

QStringList ACheckTree::getChecked() 
{
    QStringList checked;
    QString item_id;

    // Iterate across all items to tree
    QTreeWidgetItemIterator it(this);
    while( *it )
    {
	if( (*it)->checkState(0) == Qt::Checked )
	{
	    item_id = (*it)->data(0, ACHECKTREE_ID_ROLE).toString();
	    if( ! item_id.isEmpty() )
		checked.append(item_id);
	}
	it++;
    }
    return checked;
}

QString ACheckTree::current()
{
    // Return current selected item
    QList<QTreeWidgetItem*> items(selectedItems());
    if( items.size() > 0 )
	return items.first()->data(0, ACHECKTREE_ID_ROLE).toString();
    return QString();
}

// Slot for item change
void ACheckTree::onChange(QTreeWidgetItem*, int)
{
    if (eventRegistered(BrowserEventChanged))
    {
	if( !deferred_change )
	{
	    deferred_change = true;
	    QTimer::singleShot(100, this, SLOT(onChangeDeferred()));
	}
    }
}

void ACheckTree::onChangeDeferred()
{
    deferred_change = false;
    browser->emitEvent(getId(), BrowserEventChanged, AlteratorRequestDefault);
}

// Slot for item expand
void ACheckTree::onExpand(QTreeWidgetItem *item)
{
    if (item && columnCount() > 1)
    {
	for (int cc=0; cc<columnCount(); cc++)
	    resizeColumnToContents(cc);
    }
}

// Toggle checking via Space key pressed on current item
void ACheckTree::keyPressEvent(QKeyEvent * e) 
{
    if ((e->key() == Qt::Key_Space))
    {
	QTreeWidgetItem *ci = currentItem();
	if ( ci ) {
	    bool check = ci->checkState(0) == Qt::Unchecked;
	    ci->setCheckState(0, check? Qt::Checked : Qt::Unchecked);
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
	return 0;
    
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
    return 0;
}

// alCheckTree

alCheckTree::alCheckTree(const AlteratorWidgetType awtype, const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
	alWidgetPre<ACheckTree>(attr,awtype,id,parent)
{
    int col_count = 1;
    if( attr.contains("columns") ) col_count = attr.value("columns").i;
    if( col_count < 1 )
	col_count = 1;
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
	    QSet<QString> data(QSet<QString>::fromList(value.split(";")));
	    if( data.size() > 0 )
	    {
		QString id;
		QTreeWidgetItemIterator it(wnd_);
		while (*it) 
		{
		    id = (*it)->data(0, ACHECKTREE_ID_ROLE).toString();
		    if (!id.isEmpty() && data.contains(id))
		    {
			if( (*it)->checkState(0) != Qt::Checked )
			    (*it)->setCheckState(0, Qt::Checked);
		    }
		    else
		    {
			if( (*it)->checkState(0) != Qt::Unchecked )
			    (*it)->setCheckState(0, Qt::Unchecked);
		    }
		    ++it;
		}
	    }
	}
	else if ("current" == name)
	{
	    // Set item selected
	    QTreeWidgetItem* item = wnd_->lookupItem(value);
	    if ( item )
	    {
		// Expand all parent
		QTreeWidgetItem* parent_item = item->parent();
		while (parent_item) 
		{
		    parent_item->setExpanded(true);
		    parent_item = parent_item->parent();
		}
		item->setSelected(true);
	    }
	}
	else if ("icon-rows" == name)
	{
	    QString pixname;
	    int column;
	    QStringList data = value.split(";");
	    
	    if (data.count() < 2)
		return;

	    QStringListIterator it(data);
	    QString strfield(it.next());
	    // Icon name
	    QIcon pixmap = getPixmap(strfield.isEmpty()? "theme:null": strfield);
	    
	    // Column number
	    column = it.next().toInt();
	    if( column > 0 ) column--;
	    if( column < 0 ) column = 0;

	    while( it.hasNext() )
	    {
		QTreeWidgetItem* item = wnd_->lookupItem(it.next());
		if( item )
		    item->setIcon(column, pixmap);
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
    ret.append(QString(" (current . %1)").arg((wnd_->current().isEmpty()? "\"\"": wnd_->current())));
        
    // Current checked elements
    ret.append(" (current-rows . (");
    foreach(QString item, wnd_->getChecked())
    {
	ret.append(QString(" \"%1\"").arg(item));
    }
    ret.append("))");
    
    
    
    return ret;
}

