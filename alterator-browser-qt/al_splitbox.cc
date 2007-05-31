
#include "al_splitbox.hh"

ASplitBox::ASplitBox(QWidget *parent):
    QSplitter(parent)
{
    columns_.clear();
    columns_.append(100);
}

ASplitBox::~ASplitBox() {}

void ASplitBox::addChild(QWidget* chld)
{
    addWidget(chld);

    // adjust columns
    int w_num = 0;
    int all_size = 0;
    QListIterator<int> sz_it(sizes());
    while( sz_it.hasNext() )
    {
	all_size += sz_it.next();
	w_num++;
    }
    QListIterator<int> col_it(columns_);
    for(int i = 0; i < w_num; i++)
    {
	int stretch = all_size/10;
	if( col_it.hasNext() )
	    stretch = (all_size/100) * col_it.next();
	else if( i > columns_.size() && i > 0 )
	{
	    QWidget *w = widget(i-1);
	    if( w )
	    {
		QSizePolicy policy = w->sizePolicy();
		if( orientation() == Qt::Horizontal )
		    stretch = policy.horizontalStretch();
		else
		    stretch = policy.verticalStretch();
	    }
	}
	setStretchFactor(i, stretch);
    }
    qDebug("n widgets = %d", w_num);
}

void ASplitBox::setColumns(const QString &columns)
{
    if( columns_.size() > 1 )
    {
	qDebug("splitbox: Ignore double columns setup");
	return;
    }
    QStringList cols = columns.split(";", QString::SkipEmptyParts);
    QStringListIterator it(cols);
    while( it.hasNext() )
    {
	columns_.append(it.next().toInt());
    }
}

// alSplitBox
alSplitBox::alSplitBox(const QString &id,const QString &parent, Qt::Orientation orientation):
	alWidgetPre<ASplitBox>(SplitBox,id,parent)
{
    wnd_->setOrientation(Utils::fixOrientation(orientation, Qt::Horizontal));
}

void alSplitBox::addChild(QWidget *chld, alWidget::Type type)
{
    wnd_->addChild(chld);
}

void alSplitBox::setAttr(const QString& name,const QString& value)
{
    if( "columns" == name )
        wnd_->setColumns(value);
    else
        alWidget::setAttr(name,value);
}
