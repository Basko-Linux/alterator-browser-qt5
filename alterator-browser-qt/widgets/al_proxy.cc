#include "al_proxy.hh"
#include "widgetlist.hh"

alProxy::alProxy(const QString& id,const QString &parent):
	alWidget(WProxy,id,parent),
	parent_(parent)
{}

QWidget* alProxy::getWidget()
{
    alWidget *parent = widgetlist->alWidgetById(parent_);
    if( parent )
	return parent->getWidget();
    else
	return 0;
}

QLayout* alProxy::getViewLayout(void)
{
    alWidget *parent = widgetlist->alWidgetById(parent_);
    if( parent )
	return parent->getViewLayout();
    else
	return 0;
}

QWidget* alProxy::getViewWidget(void)
{
    alWidget *parent = widgetlist->alWidgetById(parent_);
    if( parent )
	return parent->getViewWidget();
    else
	return 0;
}
