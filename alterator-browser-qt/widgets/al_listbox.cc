
#include <QHeaderView>

#include "al_listbox.hh"
#include "a_pixmaps.hh"

AMultiListBox::AMultiListBox(QWidget *parent):
		QTreeWidget(parent)
{
    header()->hide();
    setUniformRowHeights(true);
    setItemsExpandable(false);
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setAllColumnsShowFocus(true);
    setIndentation(0);
    setSortingEnabled(false);

    setListType(ListBox);
    
    connect(this, SIGNAL(itemSelectionChanged()), this, SLOT(onSelectionChanged())) ;
}

AMultiListBox::~AMultiListBox()
{}

void AMultiListBox::setListType(ListType type)
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

AMultiListBox::ListType AMultiListBox::listType()
{
    return list_type;
}

void AMultiListBox::keyPressEvent(QKeyEvent * e) 
{
    if ((e->key() == Qt::Key_Space))
        emit spaceBtnPressed();
    QTreeWidget::keyPressEvent(e);
}

void AMultiListBox::showEvent(QShowEvent*)
{
    //QTreeWidget::showEvent(e);
    scrollTo(currentIndex());
}

void AMultiListBox::adjustAllColumnsWidth()
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

void AMultiListBox::setHeader(QStringList& data)
{
    if( data.size() > columnCount() )
	addRow(data, Header);
    else
	setHeaderLabels(data);
}

void AMultiListBox::addRow(QStringList& data, RowType row_type)
{
    if (data.size() < 2 ) return;

    const int columns = columnCount();
    QStringListIterator it(data);
    int col = 0;

    QTreeWidgetItem *item;
    if( row_type == Header )
	item = new QTreeWidgetItem(0);
    else
	item = new QTreeWidgetItem(this);
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
		    if(pixname == "#t")
			item->setSelected(true);
		    break;
		}
		case CheckListBox:
		{
		    if(pixname == "#t")
		    {
		        pixname = "theme:check-on";
			item->setSelected(true);
		    }
		    else
		    {
			item->setSelected(false);
			pixname = "theme:check-off";
		    }
		    break;
		}
		case RadioListBox:
		{
		    if(pixname == "#t")
		    {
			pixname = "theme:radio-on";
		    }
		    else
			pixname = "theme:radio-off";
		    break;
		}
		case ListBox:
		default:
		{
		}
	    }
	}
	item->setIcon(col, getPixmap(pixname.isEmpty()? "theme:null": pixname));
	col++;
    }
    if( row_type == Header )
	setHeaderItem(item);
}

void AMultiListBox::setRows(QStringList& data)
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

void AMultiListBox::onSelectionChanged()
{
    QList<QTreeWidgetItem*> items = selectedItems();
    if( items.size() > 0 )
    {
	switch(list_type)
	{
	    case CheckListBox:
	    case RadioListBox:
	    {
		int n = topLevelItemCount();
		if( n > 0 )
		{
		    for(int i = 0; i < n; i++)
		    {
			QTreeWidgetItem* item = topLevelItem(i);
			if( i )
			{
			    if( list_type == CheckListBox )
				item->setIcon(0, getPixmap(item->isSelected()? "theme:check-on": "theme:check-off"));
			    else
				item->setIcon(0, getPixmap(item->isSelected()? "theme:radio-on": "theme:radio-off"));
			}
		    }
		}
		break;
	    }
	    case MultiListBox:
	    case ListBox:
	    default:
	    {
	    }
	}
	emit selected();
    }
}


// alListBox

alListBox::alListBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, int cols):
	alWidgetPre<AMultiListBox>(attr,WMultiListBox,id,parent)
{
    if( cols < 1 ) cols = 1;
    wnd_->setColumnCount(cols);
    if( cols > 1 )
	wnd_->setAlternatingRowColors(true);
}

void alListBox::setAttr(const QString& name,const QString& value)
{
	if ("append-row" == name)
	{
	    QStringList data(value.split(";", QString::KeepEmptyParts));
	    wnd_->addRow(data);
	}
	if ("rows" == name)
	{
	    QStringList data(value.split(";", QString::KeepEmptyParts));
	    wnd_->setRows(data);
	}
	else if ("current" == name)
	{
		QTreeWidgetItem *i = wnd_->topLevelItem(value.toInt());
		wnd_->scrollToItem(i);
		wnd_->setCurrentItem(i);
	}
	else if ("rows-clear" == name)
	{
		wnd_->clear();
	}
	else if ("remove-row" == name)
	{
	    QTreeWidgetItem *i = wnd_->takeTopLevelItem(value.toInt());
	    if(i) delete i;
	}
	else if ("row-item-text" == name)
	{
		QStringList data = value.split(";");
		int column = data.size()<3? 0: data[2].toInt();
		wnd_->topLevelItem(data[1].toInt())->setText(column,data[0]);
	}
	else if ("row-item-pixmap" == name)
	{
		QStringList data = value.split(";");
		int column = data.size()<3? 0: data[2].toInt();
		wnd_->topLevelItem(data[1].toInt())->setIcon(column,QIcon(getPixmap(data[0])));
	}
	else if ("header" == name)
	{
		QStringList data_test = value.split(";", QString::SkipEmptyParts);
		if( data_test.size() > 0 )
		    wnd_->header()->show();
		else
		    wnd_->header()->hide();
		QStringList data = value.split(";", QString::KeepEmptyParts);
		wnd_->setHeader(data);
	}
	else
		alWidget::setAttr(name,value);
}

void alListBox::registerEvent(const QString& name)
{
	if ("selected" == name)
		connect(wnd_,SIGNAL(selected()),SLOT(onSelect()));
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

QString alListBox::postData() const
{
    QString ret;
    QString state_rows;
    QString current_rows;
    QString current;
    switch(wnd_->listType())
    {
	case AMultiListBox::MultiListBox:
	case AMultiListBox::CheckListBox:
	{
	    int n = wnd_->topLevelItemCount();
	    if( n > 0 )
	    {
		state_rows.append(" (state-rows . '(");
		for(int i = 0; i < n; i++)
		{
		    QTreeWidgetItem* item = wnd_->topLevelItem(i);
		    if( i )
			state_rows.append(item->isSelected()? " #t": " #f");
		}
		state_rows.append("))");
	    }
	    QList<QTreeWidgetItem*> items = wnd_->selectedItems();
	    if( items.size() > 0 )
	    {
		current_rows.append(" (current-rows . '(");
		foreach(QTreeWidgetItem* item, items)
		{
		    current_rows.append(QString(" %1").arg(wnd_->indexOfTopLevelItem(item)));
		}
		current_rows.append("))");
	    }
	    break;
	}
	case AMultiListBox::RadioListBox:
	case AMultiListBox::ListBox:
	default:
	{
	    QList<QTreeWidgetItem*> items = wnd_->selectedItems();
	    if( items.size() > 0 )
		current = QString(" (current . %1 )").arg(wnd_->indexOfTopLevelItem(items.first()));
	}
    }

    if(!state_rows.isEmpty())
	ret.append(state_rows);
    if(!current_rows.isEmpty())
	ret.append(current_rows);
    if(!current.isEmpty())
	ret.append(current);
    else
	ret.append(" (current . -1 )");
    return ret;
}
