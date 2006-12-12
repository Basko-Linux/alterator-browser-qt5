#ifndef QTBROWSER_HACKS_HH
#define QTBROWSER_HACKS_HH

#include <QDialog>
#include <QListWidget>
#include <QTreeWidget>

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
	void paintEvent(QPaintEvent*);
};

#endif
