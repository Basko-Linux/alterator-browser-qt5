#include "hacks.hh"

#include <QPainter>

void widget_corners_round(QWidget *widget)
{
	QPainter painter(widget);
	
	painter.drawLine(1,1,1,widget->height()-1);
	painter.drawLine(1,1,widget->width()-1,1);
	painter.drawLine(widget->width()-1,1,widget->width()-1,widget->height()-1);
	painter.drawLine(1,widget->height()-1,widget->width()-1,widget->height()-1);
}
