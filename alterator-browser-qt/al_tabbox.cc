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
		if (elements.contains(value))
			wnd_->setCurrentWidget(elements[value]->getWidget());
		else
			current_ = value;
	}
	else
		alWidget::setAttr(name,value);
}


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
