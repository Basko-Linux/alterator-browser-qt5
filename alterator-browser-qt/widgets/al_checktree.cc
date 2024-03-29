
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
    m_changed_timer = new QTimer(this);
    m_changed_timer->setSingleShot(true);
    m_selected_timer = new QTimer(this);
    m_selected_timer->setSingleShot(true);

    header()->hide();
    setUniformRowHeights(true);
    setItemsExpandable(true);
    setAllColumnsShowFocus(true);
    setSortingEnabled(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);

    // Connect to changed signal
    connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)), this, SLOT(onChange(QTreeWidgetItem*, int)));
    // Connect to selected signal
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelect()));
    // Connect to expand signal
    connect(this, SIGNAL(itemExpanded(QTreeWidgetItem *)), this, SLOT(onExpand(QTreeWidgetItem *)));

    connect(m_changed_timer, SIGNAL(timeout()), this, SLOT(onChangeDeferred()));
    connect(m_selected_timer, SIGNAL(timeout()), this, SLOT(onSelectDeferred()));

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
	    Q_FOREACH(QTreeWidgetItem *i, items)
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
    Q_FOREACH(QTreeWidgetItem *itm, orphaned.values())
    {
	addTopLevelItem(itm);
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
    {
	return items.first()->data(0, ACHECKTREE_ID_ROLE).toString();
    }
    else
    {
	QTreeWidgetItem *item(currentItem());
	if( item )
	    return item->data(0, ACHECKTREE_ID_ROLE).toString();
    }
    return QString();
}

// Slot for item change
void ACheckTree::onChange(QTreeWidgetItem*, int)
{
    if (eventRegistered(BrowserEventChanged))
    {
	m_changed_timer->start(100);
    }
}

void ACheckTree::onChangeDeferred()
{
    g_browser->emitEvent(getId(), BrowserEventChanged, AlteratorRequestDefault);
}

// Slot for item select
void ACheckTree::onSelect()
{
    if (eventRegistered(BrowserEventSelected))
    {
	m_selected_timer->start(100);
    }
}

void ACheckTree::onSelectDeferred()
{
    g_browser->emitEvent(getId(), BrowserEventSelected, AlteratorRequestDefault);
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

void ACheckTree::showEvent(QShowEvent *e)
{
    scrollTo(currentIndex());
    e->accept();
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
    if( attr.contains(QStringLiteral("columns")) ) col_count = attr.value(QStringLiteral("columns")).i;
    if( col_count < 1 )
	col_count = 1;
    wnd_->setColumnCount(col_count);
    if( col_count > 1 ) 
	wnd_->setAlternatingRowColors(true);
}

void alCheckTree::setAttr(const QString& name,const QString& value)
{
	if (QStringLiteral("append-row") == name)
	{
	    wnd_->blockSignals(true);
	    QStringList data(value.split(QLatin1Char(';'), Qt::KeepEmptyParts));
	    wnd_->addRow(data);
	    wnd_->blockSignals(false);
	}
	else if (QStringLiteral("rows") == name)
	{
	    wnd_->blockSignals(true);
	    QStringList data(value.split(QLatin1Char(';'), Qt::KeepEmptyParts));
	    wnd_->setRows(data);
	    wnd_->blockSignals(false);
	}
	else if (QStringLiteral("rows-clear") == name)
	{
	    wnd_->clear();
	}
	else if (QStringLiteral("expand-rows") == name)
	{
	    QStringList data = value.split(QLatin1Char(';'));
	    Q_FOREACH(QString id, data)
	    {
		QTreeWidgetItem* item = wnd_->lookupItem(id);
		if( item )
		    item->setExpanded(true);
	    }
	}
	else if (QStringLiteral("collapse-rows") == name)
	{
	    QStringList data = value.split(QLatin1Char(';'));
	    Q_FOREACH(QString id, data)
	    {
		QTreeWidgetItem* item = wnd_->lookupItem(id);
		if( item )
		    item->setExpanded(false);
	    }
	}
	else if (QStringLiteral("current-rows") == name)
	{
	    wnd_->blockSignals(true);
	    QStringList list(value.split(QLatin1Char(';')));
	    QSet<QString> data(list.begin(), list.end());
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
	    wnd_->blockSignals(false);
	}
	else if (QStringLiteral("current") == name)
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
	else if (QStringLiteral("icon-rows") == name)
	{
	    QString pixname;
	    int column;
	    QStringList data = value.split(QLatin1Char(';'));
	    
	    if (data.count() < 2)
		return;

	    QStringListIterator it(data);
	    QString strfield(it.next());
	    // Icon name
	    QIcon pixmap = getPixmap(strfield.isEmpty()? QStringLiteral("theme:null"): strfield);
	    
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
	if (QStringLiteral("selected") == name)
		wnd_->setEventRegistered(id_, BrowserEventSelected);
	else if (QStringLiteral("changed") == name)
		wnd_->setEventRegistered(id_, BrowserEventChanged);
	else if (QStringLiteral("clicked") == name)
	{
		connect(wnd_,SIGNAL(itemPressed(QTreeWidgetItem*,int)), SLOT(onClick(QTreeWidgetItem*,int)));
		connect(wnd_,SIGNAL(spaceBtnPressed()), SLOT(onClick()));
	}
	else if (QStringLiteral("return-pressed") == name)
		connect( wnd_, SIGNAL(itemActivated(QTreeWidgetItem*,int)), SLOT(onReturn(QTreeWidgetItem*,int)));
	else if (QStringLiteral("double-clicked") == name)
		connect(wnd_,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(onDoubleClick(QTreeWidgetItem*,int)));
}

QString alCheckTree::postData() const
{
    QString ret;
    QString selected_item_id;
    
    // Current element id
    ret.append(QString(QStringLiteral(" (current . %1)")).arg((wnd_->current().isEmpty()? QStringLiteral("\"\""): wnd_->current())));
        
    // Current checked elements
    ret.append(QStringLiteral(" (current-rows . ("));
    Q_FOREACH(QString item, wnd_->getChecked())
    {
	ret.append(QString(QStringLiteral(" \"%1\"")).arg(item));
    }
    ret.append(QStringLiteral("))"));
    
    
    
    return ret;
}

