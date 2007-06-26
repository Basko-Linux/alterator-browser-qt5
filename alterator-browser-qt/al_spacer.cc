
#include "al_spacer.hh"

alSpacer::alSpacer(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
	alWidgetPre<QWidget>(attr,Spacer,id,parent)
{
}
