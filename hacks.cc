#include "hacks.hh"

#include <QPainter>

void widget_corners_round(QWidget *widget)
{
	QPainter painter(widget);
	
	painter.drawLine(1,1,1,widget->height()-2);
	painter.drawLine(1,1,widget->width()-2,1);
	painter.drawLine(widget->width()-2,1,widget->width()-2,widget->height()-2);
	painter.drawLine(1,widget->height()-2,widget->width()-2,widget->height()-2);
}
