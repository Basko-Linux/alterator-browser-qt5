
#include "al_main_widget.hh"

alMainWidget::alMainWidget(const QString& id,const QString& parent, Qt::Orientation orientation):
	alMainWidgetPre<QWidget>(WMainWidget, id, parent)
{
    QBoxLayout *bl;
    Qt::Orientation o = Utils::fixOrientation(orientation, Qt::Vertical);
    if( o == Qt::Horizontal )
	bl = new QHBoxLayout(getViewWidget());
    else
	bl = new QVBoxLayout(getViewWidget());
    bl->setSpacing(5);
    bl->setMargin(5);
}

void alMainWidget::setAttr(const QString& name,const QString& value)
{
	if ("width" == name)
	{
		//wnd_->resize(value.toInt(),wnd_->height());
	}
	else if ("height" == name)
	{
		//wnd_->resize(wnd_->width(),value.toInt());
	}
        else if ("has-help" == name)
        {
            main_window->setHelpAvailable(value != "false");
        }
	else
		alWidget::setAttr(name,value);
}
