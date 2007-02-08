#include "al_proxy.hh"

alProxy::alProxy(const QString& id,const QString &parent):
	alWidget(Proxy,id,parent),
	parent_(parent)
{}
