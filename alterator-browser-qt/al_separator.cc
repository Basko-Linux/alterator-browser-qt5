#include "al_separator.hh"

alSeparator::alSeparator(const QString& id, const QString& parent, Qt::Orientation orientation):
    alWidgetPre<QFrame>(Separator,id,parent)
{
    QFrame::Shape orient;
    if( orientation == Qt::Vertical )
	orient = QFrame::VLine;
    else if( orientation == Qt::Horizontal )
	orient = QFrame::HLine;
    else
    {
	orient = QFrame::HLine;
	QWidget *pw = wnd_->parentWidget();
	if( pw )
	{
	    QLayout *l = pw->layout();
	    if( l )
	    {
		QBoxLayout *bl = qobject_cast<QBoxLayout*>(l);
    		if( bl )
		{
		    QBoxLayout::Direction dir = bl->direction();
		    if( dir == QBoxLayout::LeftToRight || dir == QBoxLayout::RightToLeft )
		    {
			orient = QFrame::VLine;
		    }
		}
	    }
	}
    }
    if( orient == QFrame::HLine )
	wnd_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    else
	wnd_->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    wnd_->setFrameShape(orient);
    wnd_->setFrameShadow(QFrame::Sunken);
}


void alSeparator::setAttr(const QString& name,const QString& value)
{
    alWidget::setAttr(name,value);
}
