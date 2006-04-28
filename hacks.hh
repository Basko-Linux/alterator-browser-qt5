#ifndef QTBROWSER_HACKS_HH
#define QTBROWSER_HACKS_HH

#include <QDialog>
#include <QListWidget>

#include <QKeyEvent>
#include <QPaintEvent>

#include <QFrame>
#include <QBrush>
#include <QPainter>
#include <QApplication>
#include <QDesktopWidget>

#include "main_window.hh"

extern MainWindow *main_window;

void widget_corners_round(QWidget*);

//QListWidget unable to scroll to active widget until it invisible
class QListWidget2: public QListWidget
{
public:
	QListWidget2(QWidget *parent=0):
		QListWidget(parent)
	{}
protected:
	void showEvent(QShowEvent *e)
	{
		QListWidget::showEvent(e);
		scrollTo(currentIndex());
	}
};


//hack for painting
class QFrame2: public QFrame
{
	QBrush brush_;
public:
	QFrame2(QWidget *parent=0):
		QFrame(parent),
		brush_(QApplication::palette().color(QPalette::Background))
	{}
	void setBrush(const QBrush& brush) { brush_ = brush; }
protected:
	void paintEvent(QPaintEvent* e)
	{
		QPainter p(this);
		p.fillRect(e->rect(),brush_);
	}
};

//hack to access protected done() slot
//also ignore escape button press and some other features
class QDialog2: public QDialog
{
public:
	QDialog2(QWidget *parent = 0): QDialog(parent)
	{}
protected:
	void closeEvent(QCloseEvent *e)
	{
		e->ignore();
	}
	void keyPressEvent ( QKeyEvent * e ) 
	{
		if ((e->key() != Qt::Key_Escape) && 
		    (e->key() != Qt::Key_Enter) &&
		    (e->key() != Qt::Key_Return))
			QDialog::keyPressEvent(e);
	}
	void showEvent(QShowEvent *e)
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
	void paintEvent(QPaintEvent* e)
	{
		QDialog::paintEvent(e);
    		widget_corners_round(this);
	}
};

#endif
