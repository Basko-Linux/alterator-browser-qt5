
#include "al_box.hh"

alBox::alBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation):
		alWidgetPre<QWidget>(attr,(orientation==Qt::Vertical)?WVBox:WHBox,id,parent)
{
    QBoxLayout *l;
    Qt::Orientation o = Utils::fixOrientation(orientation, Qt::Vertical);
    if( o == Qt::Horizontal )
	l = new QHBoxLayout(wnd_);
    else
	l = new QVBoxLayout(wnd_);
    l->setSpacing(5);
    l->setMargin(0);
}

void alBox::setAttr(const QString& name,const QString& value)
{
	if ("background-color" == name)
	{
		//wnd_->setBrush(QBrush(QColor(value)));
	}
	else
	    alWidget::setAttr(name,value);
}
