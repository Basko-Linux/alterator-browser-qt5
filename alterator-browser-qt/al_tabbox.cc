#include "al_tabbox.hh"

alTabBox::alTabBox(const QString& id,const QString& parent, Qt::Orientation orientation):
	alWidgetPre<QTabWidget>((orientation==Qt::Vertical)?VTabBox:HTabBox,id,parent)
{
    orientation_ = orientation;
}

void alTabBox::setAttr(const QString& name,const QString& value)
{
	if ("current" == name)
	{ //find current in elements or save value
	    wnd_->setCurrentIndex(value.toInt());
	}
	else
	    alWidget::setAttr(name,value);
}

void alTabBox::registerEvent(const QString& name)
{
       if ("selected" == name)
               connect(wnd_,SIGNAL(currentChanged(int)),SLOT(onSelect(int)));
}

QString alTabBox::postData() const
{
    return QString(" (current . %1 )").arg(wnd_->currentIndex());
}

// alTabPage

alTabPage::alTabPage(const QString& id,const QString& parent, Qt::Orientation orientation):
    alWidgetPre<QWidget>((orientation==Qt::Vertical)?VTabPage:HTabPage,id,":reparent:"+parent),
    tabbox_(getParentTabBox(parent)),
    idx_(tabbox_?static_cast<QTabWidget*>(tabbox_->getWidget())->addTab(wnd_,""):-1)
{
    Qt::Orientation orient;
    if( orientation )
	orient = orientation;
    else
	orient = tabbox_->getOrientation();
    orient = Utils::fixOrientation(orient, Qt::Vertical);

    QBoxLayout *bl;
    QWidget *vw = getViewWidget();
    if( orient == Qt::Horizontal )
	bl = new QHBoxLayout(vw);
    else
	bl = new QVBoxLayout(vw);

    bl->setSpacing(5);
    bl->setMargin(5);
}

void alTabPage::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		static_cast<QTabWidget*>(tabbox_->getWidget())->setTabText(idx_,value);
	else
		alWidget::setAttr(name,value);
}
