#include "al_tabbox.hh"
#include "widgetlist.hh"

alTabBox::alTabBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation):
	alWidgetPre<QTabWidget>(attr,(orientation==Qt::Vertical)?WVTabBox:WHTabBox,id,parent)
{
    orientation_ = orientation;
}

void alTabBox::setAttr(const QString& name,const QString& value)
{
	if ("current" == name)
	{
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

alTabPage::alTabPage(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation):
    alWidgetPre<QWidget>(attr,(orientation==Qt::Vertical)?WVTabPage:WHTabPage,id,":reparent:"+parent),
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

alTabBox* alTabPage::getParentTabBox(const QString& parent_)
{
    alTabBox *parent_tabbox = 0;
    alWidget *parent = widgetlist->alWidgetById(parent_);
    if( parent )
	parent_tabbox = dynamic_cast<alTabBox*>(parent);
    return parent_tabbox;
}

void alTabPage::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		static_cast<QTabWidget*>(tabbox_->getWidget())->setTabText(idx_,value);
	else
		alWidget::setAttr(name,value);
}
