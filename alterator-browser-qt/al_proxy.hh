#ifndef QTBROWSER_AL_PROXY_HH
#define QTBROWSER_AL_PROXY_HH

#include "al_widget.hh"

class alProxy: public alWidget
{
	QString parent_;
public:
	alProxy(const QString& id,const QString &parent);
		
protected:
	QWidget *getWidget() { return elements[parent_]->getWidget(); }
	QLayout *getViewLayout(void) { return elements[parent_]->getViewLayout(); }
	QWidget *getViewWidget(void) { return elements[parent_]->getViewWidget(); }
};

#endif
