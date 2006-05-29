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

QListWidget2::QListWidget2(QWidget *parent):
	    QListWidget(parent)
{}

QListWidget2::~QListWidget2()
{}

void QListWidget2::keyPressEvent(QKeyEvent * e) 
{
    if ((e->key() == Qt::Key_Space))
	emit spaceBtnPressed();
    QListWidget::keyPressEvent(e);
}

void QListWidget2::showEvent(QShowEvent *e)
{
    QListWidget::showEvent(e);
    scrollTo(currentIndex());
}

QTreeWidget2::QTreeWidget2(QWidget *parent):
		QTreeWidget(parent)
{}

QTreeWidget2::~QTreeWidget2()
{}

void QTreeWidget2::keyPressEvent(QKeyEvent * e) 
{
    if ((e->key() == Qt::Key_Space))
        emit spaceBtnPressed();
    QTreeWidget::keyPressEvent(e);
}

void QTreeWidget2::showEvent(QShowEvent *e)
{
    QTreeWidget::showEvent(e);
    scrollTo(currentIndex());
}
