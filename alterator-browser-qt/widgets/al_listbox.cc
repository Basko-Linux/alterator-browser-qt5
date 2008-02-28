
#include <QHeaderView>

#include "al_listbox.hh"
#include "a_pixmaps.hh"

AMultiListBox::AMultiListBox(QWidget *parent):
		QTreeWidget(parent)
{
    header()->hide();
    setSelectionBehavior(QAbstractItemView::SelectRows);
    setIndentation(0);
    setSortingEnabled(false);
}

AMultiListBox::~AMultiListBox()
{}

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

void AMultiListBox::appendRow(QStringList& data)
{
    if (data.size() < 2 ) return;


    const int columns = columnCount();
    QStringListIterator it(data);
    int col = 0;
    while( it.hasNext() && col < columns )
    {
	QString item_text = it.next();
	QString pixname;
	if( it.hasNext() )
	    pixname = it.next();
	else
	    break;

	QTreeWidgetItem *item = new QTreeWidgetItem(this);
	item->setText(col, item_text);
	if( pixname.isEmpty() )
	    item->setIcon(col, getPixmap("theme:null"));
	else
	    item->setIcon(col, getPixmap(pixname));
	col++;
    }
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
		appendRow(row_data);
		break;
	    }
	}
    }
    adjustAllColumnsWidth();
}

// alListBox

alListBox::alListBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, int cols):
	alWidgetPre<AMultiListBox>(attr,WMultiListBox,id,parent)
{
    if( cols < 1 ) cols = 1;
    //setings to be compatible with QListView
    wnd_->setColumnCount(cols);
    if( cols > 1 )
	wnd_->setAlternatingRowColors(true);
}

void alListBox::setAttr(const QString& name,const QString& value)
{
	if ("append-row" == name)
	{
	    QStringList data(value.split(";", QString::KeepEmptyParts));
	    wnd_->appendRow(data);
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
		delete wnd_->takeTopLevelItem(value.toInt());
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
		wnd_->header()->show();
		wnd_->setHeaderLabels(value.split(";"));
	}
	else
		alWidget::setAttr(name,value);
}

void alListBox::registerEvent(const QString& name)
{
	if ("selected" == name)
		connect(wnd_,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
			     SLOT(onSelect(QTreeWidgetItem*,QTreeWidgetItem*)));
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
	QTreeWidgetItem *i = wnd_->currentItem();
	if( i )
	    return QString(" (current . %1 )").arg(wnd_->indexOfTopLevelItem(i));
	else
	    return "";
}
