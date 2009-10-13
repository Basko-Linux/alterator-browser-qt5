
#include "al_splitbox.hh"

ASplitBox::ASplitBox(QWidget *parent, const Qt::Orientation):
    AWidget<QSplitter>(parent)
{
    columns_.clear();
    columns_.append(100);
    columns_sum = 1;
}

ASplitBox::~ASplitBox() {}

void ASplitBox::postAddChild(QWidget* chld)
{
    addWidget(chld);

    int w_num = count();
    int col_num = columns_.size();

    // compute undefined columns
    if( w_num > col_num )
    {
	int stretch = (100-columns_sum)/(w_num-col_num);
	if( stretch < 0 ) stretch = 1;
	for(int i = col_num; i < w_num; i++)
	{
	    columns_add_.append(stretch);
	}
    }
}

void ASplitBox::showEvent(QShowEvent *e)
{
    int w_num = count();
    int col_num = columns_.size();
    int all_size = (orientation() == Qt::Vertical )? height(): width(); 
    QList<int> tmp_cols;
    int cols_sz = 0;

    // defined columns
    QListIterator<int> col_it(columns_);
    int c = 0;
    while( col_it.hasNext() && c < w_num )
    {
	int sz = (all_size/100)*col_it.next();
	cols_sz += sz;
	tmp_cols.append(sz);
	c++;
    }

    // undefined columns
    if( w_num > col_num )
    {
	int sz = (all_size-cols_sz)/(w_num-col_num);
	if( sz <= 0 ) sz = 10;
	for(int i = col_num; i < w_num; i++)
	{
	    if( c >= w_num ) break;
	    tmp_cols.append(sz);
	}
    }

    // set widget sizes
    setSizes(tmp_cols);
}

void ASplitBox::setColumns(const QString &columns)
{
    if( columns_.size() > 1 )
    {
	qDebug("splitbox: Ignore double columns setup");
	return;
    }
    columns_.clear();
    columns_sum = 0;
    QStringList cols = columns.split(";", QString::SkipEmptyParts);
    QStringListIterator it(cols);
    while( it.hasNext() )
    {
	int col_sz = it.next().toInt();
	if( col_sz < 0 ) col_sz = 0;
	columns_.append(col_sz);
	columns_sum += col_sz;
    }
    if( columns_sum <= 0 )
	columns_sum = 1;
}

// alSplitBox
alSplitBox::alSplitBox(const AlteratorRequestActionAttrs &attr, const QString &id,const QString &parent, const QString &columns):
	alWidgetPre<ASplitBox>(attr,WSplitBox,id,parent)
{
    wnd_->setOrientation(Utils::fixOrientation(attr.value("orientation").o, Qt::Horizontal));
    if( !columns.isEmpty() )
	wnd_->setColumns(columns);
}

void alSplitBox::postAddChild(QWidget *chld, AlteratorWidgetType type, const AlteratorRequestActionAttrs&)
{
    wnd_->postAddChild(chld);
}

void alSplitBox::setAttr(const QString& name,const QString& value)
{
    if( "columns" == name )
    {
        wnd_->setColumns(value);
    }
    else
        alWidget::setAttr(name,value);
}
