#include "al_splitter.hh"

alSplitter::alSplitter(const QString& id, const QString& parent):
    alWidgetPre<QFrame>(Splitter,id,parent)
{
    QFrame::Shape orient = QFrame::HLine;
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
    wnd_->setFrameShape(orient);
    wnd_->setFrameShadow(QFrame::Sunken);
}


void alSplitter::setAttr(const QString& name,const QString& value)
{
# if 0
	if( "orientation" == name )
	{
	    if( "vertical" == value )
		wnd_->setFrameShape(QFrame::VLine);
	    else
		wnd_->setFrameShape(QFrame::HLine);
	}
	else
#endif
	    alWidget::setAttr(name,value);
}
