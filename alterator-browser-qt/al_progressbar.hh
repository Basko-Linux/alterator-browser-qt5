#ifndef QTBROWSER_AL_PROGRESSBAR_HH
#define QTBROWSER_AL_PROGRESSBAR_HH

#include <QProgressBar>

#include "al_widget.hh"

class alProgressBar: public alWidgetPre<QProgressBar>
{
public:
    alProgressBar(const QString& id,const QString& parent):
	alWidgetPre<QProgressBar>(ProgressBar,id,parent)
    {}
protected:
    void setAttr(const QString& name,const QString& value);
};

#endif
