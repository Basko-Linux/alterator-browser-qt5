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

void QDialog2::showEvent(QShowEvent *e)
{
    if( !main_window->haveWindowManager() )
    {
	QDialog::showEvent(e);
	int x = QApplication::desktop()->width()/2 - width()/2;
	if( x < 0 ) x = 0;
	int y = QApplication::desktop()->height()/2 - height()/2;
	if( y < 0 ) y = 0;
	move(x, y);
	QCursor::setPos(mapToGlobal(QPoint(width()/2, height()/2)));
    }
}

void QDialog2::paintEvent(QPaintEvent* e)
{
    if( !main_window->haveWindowManager() )
    {
	QDialog::paintEvent(e);
	widget_corners_round(this);
    }
}

void QDialog2::keyPressEvent ( QKeyEvent * e ) 
{
	if ((e->key() != Qt::Key_Escape) && 
	    (e->key() != Qt::Key_Enter) &&
	    (e->key() != Qt::Key_Return))
		QDialog::keyPressEvent(e);
}

void QDialog2::closeEvent(QCloseEvent *e)
{
    e->ignore();
}

void QFrame2::paintEvent(QPaintEvent* e)
{
    QPainter p(this);
    p.fillRect(e->rect(),brush_);
}
