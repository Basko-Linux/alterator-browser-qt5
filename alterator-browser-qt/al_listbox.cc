
#include <QHeaderView>

#include "al_listbox.hh"
#include "a_pixmaps.hh"

ATreeWidget::ATreeWidget(QWidget *parent):
		QTreeWidget(parent)
{}

ATreeWidget::~ATreeWidget()
{}

void ATreeWidget::keyPressEvent(QKeyEvent * e) 
{
    if ((e->key() == Qt::Key_Space))
        emit spaceBtnPressed();
    QTreeWidget::keyPressEvent(e);
}

void ATreeWidget::showEvent(QShowEvent *e)
{
    //QTreeWidget::showEvent(e);
    scrollTo(currentIndex());
}

void ATreeWidget::adjustAllColumnsWidth()
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

// alMultiListBox

alMultiListBox::alMultiListBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, int cols):
	alWidgetPre<ATreeWidget>(attr,MultiListBox,id,parent)
{
    if( cols < 1 ) cols = 1;
    //setings to be compatible with QListView
    wnd_->setColumnCount(cols);
    wnd_->header()->hide();
    wnd_->setSelectionBehavior(QAbstractItemView::SelectRows);
    wnd_->setIndentation(0);
    if( cols > 1 )
	wnd_->setAlternatingRowColors(true);
}

void alMultiListBox::setAttr(const QString& name,const QString& value)
{
	if ("append-row" == name)
	{
		QStringList data = value.split(";");
		const int len = data.size();
		if (len <= 1) return;
		const int columns = wnd_->columnCount();
		QTreeWidgetItem *item(new QTreeWidgetItem(wnd_));
		for (int col = 0;(col < columns*2) && (col+1 < len);col+=2)
		{
			if (!data[col].isEmpty())
				item->setText(col/2,data[0]);
			if (!data[col+1].isEmpty())
				item->setIcon(col/2,QIcon(getPixmap(data[col+1])));
			else
				item->setIcon(col/2,QIcon(getPixmap("theme:null")));
		}
	}
	if ("rows" == name)
	{
		QStringList data = value.split(";");
		QList<QTreeWidgetItem *> items;
		const int len = data.size();
		const int columns = wnd_->columnCount();
		for(int i=0;i+1 < len;)
		{
			QTreeWidgetItem *elt = new QTreeWidgetItem(0);
			for(int col = 0; (col < columns);++col,i+=2)
			{
				if (!data[i].isEmpty())
					elt->setText(col,data[i]);
				if (!data[i+1].isEmpty())
					elt->setIcon(col,QIcon(getPixmap(data[i+1])));
				else
					elt->setIcon(col,QIcon(getPixmap("theme:null")));
			}
			//items.push_front(elt);
			items.push_back(elt);
		}
		wnd_->clear();		
		wnd_->addTopLevelItems(items);
		wnd_->adjustAllColumnsWidth();
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

void alMultiListBox::registerEvent(const QString& name)
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

QString alMultiListBox::postData() const
{
	QTreeWidgetItem *i = wnd_->currentItem();
	if( i )
	    return QString(" (current . %1 )").arg(wnd_->indexOfTopLevelItem(i));
	else
	    return "";
}
