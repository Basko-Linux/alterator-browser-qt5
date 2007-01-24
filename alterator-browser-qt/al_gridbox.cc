
#include <QChildEvent>

#include "al_gridbox.hh"

AGridBox::AGridBox(QWidget *parent):
    QWidget(parent)
{
    columns_ = 0;
    current_row = 0;
    current_column = 0;

    view_widget = new QWidget(this);
    view_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QSpacerItem *top = new QSpacerItem(50, 50, QSizePolicy::Expanding, QSizePolicy::Expanding);
    QSpacerItem *bottom = new QSpacerItem(50, 50, QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *layout_main = new QVBoxLayout(this);
    layout_main->setSpacing(0);
    layout_main->setMargin(0);

    layout_ = new QGridLayout(view_widget);
    layout_->setMargin(0);
    layout_->setSpacing(5);

    layout_main->addItem(top);
    layout_main->addWidget(view_widget);
    layout_main->addItem(bottom);
}

AGridBox::~AGridBox() {}

QWidget* AGridBox::getView()
{
    return view_widget;
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

void AGridBox::addChild(QWidget* chld)
{
	    QWidget *w = chld;
	    if( w )
	    {
		if( columns_ <= 0 )
		{
		    //qDebug("gridbox: set default 1 column");
		    setColumns("100");
		}
		layout_->addWidget(w, current_row, current_column);

		current_column++;
		if( current_column >= columns_ )
		{
		    current_column = 0;
		    current_row++;
		}
	    }
}

// alGridBox
alGridBox::alGridBox(const QString &id,const QString &parent, const QString &columns):
	alWidgetPre<AGridBox>(GridBox,id,parent)
{
    if( !columns.isEmpty() )
	wnd_->setColumns(columns);
}

void alGridBox::setAttr(const QString &name,const QString &value)
{
    if ("columns" == name)
	wnd_->setColumns(value);
    else
	alWidget::setAttr(name,value);
}

QWidget* alGridBox::getViewWidget()
{
    return wnd_->getView();
}

QLayout* alGridBox::getViewLayout()
{
    return wnd_->getView()->layout();
}

void alGridBox::addChild(QWidget *chld, alWidget::Type type)
{
    wnd_->addChild(chld);
}
