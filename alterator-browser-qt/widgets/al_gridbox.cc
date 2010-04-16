
#include "al_gridbox.hh"

AGridBox::AGridBox(QWidget *parent, const Qt::Orientation):
    AWidget<QWidget>(parent)
{
    columns_ = 0;
    current_row = 0;
    current_column = 0;

    layout_main = new QVBoxLayout(this);
    layout_main->setSpacing(0);
    layout_main->setMargin(0);

    layout_ = new QGridLayout();
    layout_->setMargin(0);
    layout_->setSpacing(5);

    layout_main->addStretch(1);
    layout_main->addLayout(layout_);
    layout_main->addStretch(1);
}

AGridBox::~AGridBox() {}

QLayout* AGridBox::getViewLayout()
{
    return layout_;
}

void AGridBox::setColumns(const QString &columns)
{
    if( columns_ > 0 )
    {
	qDebug("gridbox: Ignore double layout setup");
	return;
    }
    QStringList cols = columns.split(";", QString::SkipEmptyParts);
    columns_ = cols.size();
    for(int i=0; i < columns_; i++)
    {
	//qDebug("gridbox::setColumns: %d column stretsh %d", i, cols.at(i).toInt());
	layout_->setColumnStretch(i, cols.at(i).toInt() );
    }
    if( columns_ <= 0 )
	columns_ = 1;
    current_row = 0;
    current_column = 0;
    //qDebug("gridbox::setColumns: %d columns", columns_);
}

void AGridBox::postAddChild(QWidget* chld, int rowspan, int colspan)
{
    int rows = rowspan; if ( rows <= 0 ) rows = 1;
    int cols = colspan; if ( cols <= 0 ) cols = 1;

    QWidget *w = chld;
    if( w )
    {
	if( columns_ <= 0 )
	{
	    //qDebug("gridbox: set default 1 column");
	    setColumns("100");
	}

	// check if occuped
	for(int c = 0; c < cols; c++)
	{
	    for(int r = 0; r < rows; r++)
	    {
		while( occuped.contains(qMakePair(current_column+c,current_row+r)) )
		{
		    current_column++;
		    if( current_column >= columns_ )
		    {
			current_column = 0; current_row++;
			c = 0; r++;
		    }
		}
	    }
	}

	layout_->addWidget(w, current_row, current_column, rows, cols);
	{ // adjustPlacementPolycy
	    const char *class_name = w->metaObject()->className();
	    QSizePolicy p = w->sizePolicy();
	    if( qstrcmp("QPushButton", class_name) == 0 )
	    {
		p.setHorizontalPolicy(QSizePolicy::Maximum);
		layout_->setAlignment(w, Qt::AlignHCenter);
	    }
	    w->setSizePolicy(p);
	}

	// remember occuped cells
	if( rows > 1 )
	{
	    for(int c = 0; c < cols; c++)
	    {
		for(int r = 0; r < rows; r++)
		{
		    occuped.append(qMakePair(current_column+c, current_row+r));
		}
	    }
	}

	current_column += cols;
	if( current_column >= columns_ )
	{
	    current_column = 0;
	    current_row++;
	}
    }
}

void AGridBox::setExpanded(bool expand)
{
    int n = layout_main->count();
    for(int i=0; i<n; i++)
    {
	QLayoutItem *item = layout_main->itemAt(i);
	if( item && item->spacerItem() )
	    layout_main->removeItem(item);
    }
    if( !expand )
    {
	layout_main->insertStretch(0);
	layout_main->insertStretch(-1);
    }
}

// alGridBox
alGridBox::alGridBox(const AlteratorRequestActionAttrs &attr, const QString &id,const QString &parent):
	alWidgetPre<AGridBox>(attr,WGridBox,id,parent)
{
    if( attr.contains("columns") )
	wnd_->setColumns(attr.value("columns").s);
}

void alGridBox::setAttr(const QString &name,const QString &value)
{
    if ("columns" == name)
	wnd_->setColumns(value);
    else if ("expanded" == name)
	wnd_->setExpanded(value == "#t");
    else
	alWidget::setAttr(name,value);
}

QLayout* alGridBox::getViewLayout()
{
    return wnd_->getViewLayout();
}

void alGridBox::postAddChild(QWidget *chld, AlteratorWidgetType, const AlteratorRequestActionAttrs& attr)
{
    wnd_->postAddChild(chld, attr.value("rowspan").i, attr.value("colspan").i);
}
