
#include "al_box.hh"

alBox::alBox(const QString& id,const QString& parent, Qt::Orientation orientation):
		alWidgetPre<QWidget>((orientation==Qt::Vertical)?VBox:HBox,id,parent)
{
    QBoxLayout *l;
    if( orientation == Qt::Horizontal )
	l = new QHBoxLayout(wnd_);
    else
	l = new QVBoxLayout(wnd_);
    l->setSpacing(5);
    l->setMargin(0);
}

void alBox::setAttr(const QString& name,const QString& value)
{
	if ("margin" == name)
	{
	}
	else if ("spacing" == name)
	{
	}
	else if("clear-layout" == name)
	{
	    QLayout *l = getViewLayout();
	    if( l )
	    {
		for (int i = 0; i < l->count(); ++i)
		    delete l->takeAt(i);
	    }
	}
	else if ("background-color" == name)
	{
		//wnd_->setBrush(QBrush(QColor(value)));
	}
	else
	    alWidget::setAttr(name,value);
}
