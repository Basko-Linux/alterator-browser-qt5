#ifndef QTBROWSER_AL_PROXY_HH
#define QTBROWSER_AL_PROXY_HH

#include "al_widget.hh"

class alProxy: public alWidget
{
	QString parent_;
public:
	alProxy(const QString& id,const QString &parent);
		
protected:
	QWidget *getWidget();
	QLayout *getViewLayout(void);
	QWidget *getViewWidget(void);
};

#endif
