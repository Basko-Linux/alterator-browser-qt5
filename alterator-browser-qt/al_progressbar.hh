#ifndef QTBROWSER_AL_PROGRESSBAR_HH
#define QTBROWSER_AL_PROGRESSBAR_HH

#include <QProgressBar>

#include "al_widget.hh"

class alProgressBar: public alWidgetPre<QProgressBar>
{
public:
    alProgressBar(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
	alWidgetPre<QProgressBar>(attr,WProgressBar,id,parent)
    {}
protected:
    void setAttr(const QString& name,const QString& value);
};

#endif
