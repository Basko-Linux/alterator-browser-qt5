
#include "al_main_widget.hh"

void alMainWidget::setAttr(const QString& name,const QString& value)
{
	if ("title" == name)
	{
		wnd_->setWindowTitle(value);
	}
	else if ("full-screen" == name)
	{
	    main_window->setFullScreen("true" == value);
	}
	else if ("layout-policy" == name)
	{
		//qDebug("don't do layout policy for main widget");
	}
	else if ("width" == name)
	{
		//wnd_->resize(value.toInt(),wnd_->height());
	}
	else if ("height" == name)
	{
		//wnd_->resize(wnd_->width(),value.toInt());
	}
	else
		alWidget::setAttr(name,value);
}

