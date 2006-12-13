#include "al_tabbox.hh"

void alTabPage::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		static_cast<QTabWidget*>(tabbox_->getWidget())->setTabText(idx_,value);
	else
		alWidget::setAttr(name,value);
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

