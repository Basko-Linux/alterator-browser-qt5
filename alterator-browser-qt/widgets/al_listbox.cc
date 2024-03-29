
#include <QHeaderView>

#include "al_listbox.hh"
#include "a_pixmaps.hh"

AListBoxItem::AListBoxItem(QTreeWidget *parent):
    QTreeWidgetItem(parent)
{
}

AListBoxItem::~AListBoxItem()
{
    QTreeWidget *t = treeWidget();
    if(t)
    {
	AListBox *p = qobject_cast<AListBox*>(treeWidget());
	if(p)
	    p->removeFromSelectedItemsListOld(this);
	else
	    qWarning("AListBoxItem parent is not AListBox");
    }
}

AListBox::AListBox(QWidget *parent, const Qt::Orientation):
    AWidget<QTreeWidget>(parent)
{
    nonuser_selection_change = false;
    header()->hide();
    setUniformRowHeights(true);
    setItemsExpandable(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAllColumnsShowFocus(true);
    setIndentation(0);
    setSortingEnabled(false);

    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged())) ;
}

AListBox::~AListBox()
{}

void AListBox::removeFromSelectedItemsListOld(QTreeWidgetItem *i)
{
    selected_items_old.removeAll(i);
}

void AListBox::setListType(ListType type)
{
    list_type = type;
    QAbstractItemView::SelectionMode selection_mode = QAbstractItemView::SingleSelection;
    switch(list_type)
    {
	case MultiListBox:
	{
	    selection_mode = QAbstractItemView::ExtendedSelection;
	    break;
	}
	case CheckListBox:
	{
	    selection_mode = QAbstractItemView::MultiSelection;
	    break;
	}
	case RadioListBox:
	case ListBox:
	default:
	{
	    selection_mode = QAbstractItemView::SingleSelection;
	}
    }
    setSelectionMode(selection_mode);
}

AListBox::ListType AListBox::listType()
{
    return list_type;
}

void AListBox::keyPressEvent(QKeyEvent * e) 
{
    if ((e->key() == Qt::Key_Space))
    {
	QTreeWidgetItem *ci = currentItem();
	if( ci )
	    ci->setSelected(!ci->isSelected());
        Q_EMIT spaceBtnPressed();
    }
    else
	QTreeWidget::keyPressEvent(e);
}

void AListBox::showEvent(QShowEvent *e)
{
    scrollTo(currentIndex());
    e->accept();
}

void AListBox::adjustAllColumnsWidth()
{
    int n_columns = columnCount();
    if( n_columns > 0 )
    {
	for(int col=0;col < n_columns; col++)
	{
	    resizeColumnToContents(col);
	}
    }
}

void AListBox::setHeader(QStringList& data)
{
    if( data.size() > columnCount() )
	addRow(data, Header);
    else
	setHeaderLabels(data);
}

void AListBox::addRow(QStringList& data, RowType row_type)
{
    if (data.size() < 2 ) return;

    const int columns = columnCount();
    QStringListIterator it(data);
    int col = 0;

    AListBoxItem *item = new AListBoxItem(this);
    while( it.hasNext() && col < columns )
    {
	QString item_text = it.next();
	QString pixname;
	if( it.hasNext() )
	    pixname = it.next();
	else
	    break;

	item->setText(col, item_text);
	if( col == 0 )
	{

	    switch(list_type)
	    {
		case MultiListBox:
		{
		    if(pixname == QStringLiteral("true"))
			item->setSelected(true);
		    break;
		}
		case CheckListBox:
		{
		    if(pixname == QStringLiteral("true"))
		    {
		        pixname = QStringLiteral("theme:check-on");
			item->setSelected(true);
		    }
		    else
		    {
			item->setSelected(false);
			pixname = QStringLiteral("theme:check-off");
		    }
		    break;
		}
		case RadioListBox:
		{
		    if(pixname == QStringLiteral("true"))
		    {
			pixname = QStringLiteral("theme:radio-on");
		    }
		    else
			pixname = QStringLiteral("theme:radio-off");
		    break;
		}
		case ListBox:
		default:
		{
		}
	    }
	}
	item->setIcon(col, getPixmap(pixname.isEmpty()? QStringLiteral("theme:null"): pixname));
	col++;
    }
    if( row_type == Header )
	setHeaderItem(item);
}

void AListBox::setRows(QStringList& data)
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
	    if( row_data.size() >= columns*2 )
	    {
		addRow(row_data);
		break;
	    }
	}
    }
    adjustAllColumnsWidth();
}

void AListBox::setNonUserSelectionChange(bool is)
{
    nonuser_selection_change = is;
}

void AListBox::onSelectionChanged()
{

	switch(list_type)
	{
	    case CheckListBox:
	    case RadioListBox:
	    {
		QList<QTreeWidgetItem*> selected_items = selectedItems();
		Q_FOREACH(QTreeWidgetItem *i, selected_items)
		{
		    if( !selected_items_old.contains(i) )
			i->setIcon(0, getPixmap((list_type == CheckListBox)? QStringLiteral("theme:check-on"): QStringLiteral("theme:radio-on")));
		}
		Q_FOREACH(QTreeWidgetItem *oi, selected_items_old)
		{
		    if( !selected_items.contains(oi) )
			oi->setIcon(0, getPixmap((list_type == CheckListBox)? QStringLiteral("theme:check-off"): QStringLiteral("theme:radio-off")));
		}
		selected_items_old = selected_items;
		break;
	    }
	    case MultiListBox:
	    case ListBox:
	    default:
	    {
	    }
	}

    if( !nonuser_selection_change )
    {
	Q_EMIT selected();
    }
}


// alListBox

alListBox::alListBox(const AlteratorWidgetType awtype, const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
	alWidgetPre<AListBox>(attr,awtype,id,parent)
{
    int cols = 1;
    if( attr.contains(QStringLiteral("columns")) )
	cols = attr.value(QStringLiteral("columns")).i;
    if( cols < 1 ) cols = 1;
    wnd_->setColumnCount(cols);
    if( cols > 1 )
	wnd_->setAlternatingRowColors(true);
    AListBox::ListType list_type = AListBox::ListBox;
    switch(awtype)
    {
	case WMultiListBox: {list_type = AListBox::MultiListBox; break;}
	case WCheckListBox: {list_type = AListBox::CheckListBox; break;}
	case WRadioListBox: {list_type = AListBox::RadioListBox; break;}
	case WListBox:
	default:
	{
	    list_type = AListBox::ListBox;
	}
    }
    wnd_->setListType(list_type);
}

void alListBox::setAttr(const QString& name,const QString& value)
{
	if (QStringLiteral("append-row") == name)
	{
	    QStringList data(value.split(QLatin1Char(';'), Qt::KeepEmptyParts));
	    wnd_->addRow(data);
	}
	else if (QStringLiteral("rows") == name)
	{
	    QStringList data(value.split(QLatin1Char(';'), Qt::KeepEmptyParts));
	    wnd_->setRows(data);
	}
	else if (QStringLiteral("current") == name)
	{
	    wnd_->setNonUserSelectionChange(true);
		QTreeWidgetItem *i = wnd_->topLevelItem(value.toInt());
		wnd_->clearSelection();
		wnd_->setCurrentItem(i);
		if( wnd_->selectionMode() != QAbstractItemView::SingleSelection )
		    i->setSelected(i);
		wnd_->scrollToItem(i);
	    wnd_->setNonUserSelectionChange(false);
	}
	else if (QStringLiteral("current-rows") == name)
	{
	    wnd_->setNonUserSelectionChange(true);
	    wnd_->clearSelection();
	    QStringList data = value.split(QLatin1Char(';'));
	    int n = data.size();
	    if( n > 0 )
	    {
		if( wnd_->selectionMode() == QAbstractItemView::SingleSelection )
		{
		    QString sfirst = data.first();
		    data.clear();
		    data << sfirst;
		}
		Q_FOREACH(QString sidx, data)
		{
		    bool ok;
		    int idx = sidx.toInt(&ok);
		    if(ok && idx >= 0)
		    {
		        QTreeWidgetItem *item = wnd_->topLevelItem(idx);
		        if( item )
			    item->setSelected(true);
		    }
		}
	    }
	    wnd_->setNonUserSelectionChange(false);
	}
	else if (QStringLiteral("state-rows") == name)
	{
	    wnd_->setNonUserSelectionChange(true);
	    QStringList data = value.split(QLatin1Char(';'));
	    int n = wnd_->topLevelItemCount();
	    if( n == data.size() )
	    {
		int i = 0;
		Q_FOREACH(QString sidx, data)
		{
		    QTreeWidgetItem* item = wnd_->topLevelItem(i);
		    if( item )
			item->setSelected((sidx==QStringLiteral("true"))? true: false);
		    if(i >= n) break;
		    i++;
		}
	    }
	    wnd_->setNonUserSelectionChange(false);
	}
	else if (QStringLiteral("rows-clear") == name)
	{
		wnd_->clear();
	}
	else if (QStringLiteral("remove-row") == name)
	{
	    QTreeWidgetItem *i = wnd_->takeTopLevelItem(value.toInt());
	    if(i) delete i;
	}
	else if (QStringLiteral("row-item-text") == name)
	{
		QStringList data = value.split(QLatin1Char(';'));
		int column = data.size()<3? 0: data[2].toInt();
		wnd_->topLevelItem(data[1].toInt())->setText(column,data[0]);
	}
	else if (QStringLiteral("row-item-pixmap") == name)
	{
		QStringList data = value.split(QLatin1Char(';'));
		int column = data.size()<3? 0: data[2].toInt();
		wnd_->topLevelItem(data[1].toInt())->setIcon(column,QIcon(getPixmap(data[0])));
	}
	else if (QStringLiteral("header") == name)
	{
		QStringList data_test = value.split(QLatin1Char(';'), Qt::SkipEmptyParts);
		if( data_test.size() > 0 )
		    wnd_->header()->show();
		else
		    wnd_->header()->hide();
		QStringList data = value.split(QLatin1Char(';'), Qt::KeepEmptyParts);
		wnd_->setHeader(data);
	}
	else
		alWidget::setAttr(name,value);
}

void alListBox::registerEvent(const QString& name)
{
	if (QStringLiteral("selected") == name)
		connect(wnd_,SIGNAL(selected()),SLOT(onSelect()));
	else if (QStringLiteral("changed") == name)
		connect(wnd_,SIGNAL(selected()),SLOT(onChange()));
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

QString alListBox::postData() const
{
    QString ret;
    switch(wnd_->listType())
    {
	case AListBox::MultiListBox:
	case AListBox::CheckListBox:
	{
	    QString cur_rows;
	    QList<QTreeWidgetItem*> selected_items = wnd_->selectedItems();
	    Q_FOREACH(QTreeWidgetItem* item, selected_items)
	    {
		cur_rows.append(QString(QStringLiteral(" %1")).arg(wnd_->indexOfTopLevelItem(item)));
	    }
	    QString st_rows;
	    int n = wnd_->topLevelItemCount();
	    if( n > 0 )
	    {
		for(int i = 0; i < n; i++)
		{
		    QTreeWidgetItem* item = wnd_->topLevelItem(i);
		    if( item )
		    {
			st_rows.append(selected_items.contains(item)? QStringLiteral(" #t"): QStringLiteral(" #f"));
		    }
		}
	    }
	    ret.append(QStringLiteral(" (state-rows . ("));
	    ret.append(st_rows);
	    ret.append(QStringLiteral("))"));
	    ret.append(QStringLiteral(" (current-rows . ("));
	    ret.append(cur_rows);
	    ret.append(QStringLiteral("))"));
	    break;
	}
	case AListBox::RadioListBox:
	case AListBox::ListBox:
	default:
	{
	    int cur = -1;
	    QList<QTreeWidgetItem*> items = wnd_->selectedItems();
	    if( items.size() > 0 )
		cur = wnd_->indexOfTopLevelItem(items.first());
	    if( cur < 0 ) {
		QTreeWidgetItem *cur_i = wnd_->currentItem();
		if(cur_i)
		    cur = wnd_->indexOfTopLevelItem(cur_i);
	    }
	    ret.append(QString(QStringLiteral(" (current . %1 )")).arg(cur));
	}
    }
    return ret;
}
