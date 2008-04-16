
#include "al_box.hh"

alBox::alBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation):
		alWidgetPre<ASimpleBox>(attr,(orientation==Qt::Vertical)?WVBox:WHBox,id,parent)
{
    qDebug("vbox, hbox, box widgets are deprecated. Use gridbox and widgets with colspan/rowspan attributes.");

    QBoxLayout *l;
    Qt::Orientation o = Utils::fixOrientation(orientation, Qt::Vertical);
    if( o == Qt::Horizontal )
	l = new QHBoxLayout(wnd_);
    else
	l = new QVBoxLayout(wnd_);
    l->setSpacing(5);
    l->setMargin(0);
}
