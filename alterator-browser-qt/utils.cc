#include <QMessageBox>
#include <QApplication>
#include <QPainter>
#include <QColorGroup>
#include <QDesktopWidget>

#include "utils.hh"


namespace Utils
{

QString reparentTag(const QString &parent)
{
    QString ret(parent);
    return ret.replace(QString(":reparent:"),QString(""));
}

Qt::Alignment convertAlign(const QString& value)
{
	if ("left" == value) return  Qt::AlignLeft;
	else if ("right" == value) return Qt::AlignRight;
	else if ("center" == value) return Qt::AlignCenter;
	return Qt::AlignJustify;
}

QLineEdit::EchoMode convertEchoMode(const QString& value)
{
	if ("no" == value) return QLineEdit::NoEcho;
	else if ("stars" == value) return QLineEdit::Password;
	return QLineEdit::Normal; //default yes
}

QString simpleQuote(const QString &s)
{
    QString ret(s);
    return ret.replace("\\","\\\\").replace("\"","\\\"");
}

void errorExit(QWidget *parent, const QString& message)
{
    qDebug("error exit:%s",qPrintable(message));
    QMessageBox::critical(parent, QMessageBox::tr("Error"),
	message,
	QMessageBox::Abort);
    //QApplication::closeAllWindows();
    exit(1);
}

void widgetCornersRound(QWidget *widget)
{
    int framesize_ = 8;

    QPainter painter(widget);
    QPalette palette = widget->palette();

    QBrush bg_brush = palette.brush( QPalette::Background );
    QColor dark_color = palette.dark().color();
    // top
    QRect frame(0, 0, widget->width(), framesize_);
    painter.fillRect(frame, bg_brush);
    // left
    frame.setRect(0, 0, framesize_, widget->height());
    painter.fillRect(frame, bg_brush);
    // bottom
    frame.setRect(0, widget->height() - (framesize_*2), widget->width(),  (framesize_*2));
    painter.fillRect(frame, bg_brush);
    // right
    frame.setRect(widget->width()-framesize_, 0, framesize_, widget->height());
    painter.fillRect(frame, bg_brush);

    // outline the frame
    //painter.setPen( dark_color );
    painter.setPen( QColor("black") );
    frame = widget->rect();
    frame.setHeight( frame.height()-1 );
    frame.setWidth( frame.width()-1 );
    painter.drawRect(frame);

    // set the inner frame
    painter.setPen( dark_color );
    frame.setRect(frame.x() + framesize_-1, frame.y() + framesize_-1,
	frame.width() - framesize_*2 +2, frame.height() - (framesize_*2) +2);

    // local temp right and bottom
    int r(widget->width());
    int b(widget->height());
    // draw the inner frame with rounded corners
    painter.setPen( dark_color );
    painter.drawRoundRect(frame, 1, 1);

    // choose a color from scheme
    // painter.setPen( dark_color );
    painter.setPen( QColor("black") );

    // Draw edge of top-left corner inside the area removed by the mask.
    painter.drawPoint(4, 1);
    painter.drawPoint(3, 1);
    painter.drawPoint(2, 2);
    painter.drawPoint(1, 3);
    painter.drawPoint(1, 4);

    // Draw edge of top-right corner inside the area removed by the mask.
    painter.drawPoint(r - 5, 1);
    painter.drawPoint(r - 4, 1);
    painter.drawPoint(r - 3, 2);
    painter.drawPoint(r - 2, 3);
    painter.drawPoint(r - 2, 4);

    // Draw edge of bottom-left corner inside the area removed by the mask.
    painter.drawPoint(1, b - 5);
    painter.drawPoint(1, b - 4);
    painter.drawPoint(2, b - 3);
    painter.drawPoint(3, b - 2);
    painter.drawPoint(4, b - 2);

    // Draw edge of bottom-right corner inside the area removed by the mask.
    painter.drawPoint(r - 2, b - 5);
    painter.drawPoint(r - 2, b - 4);
    painter.drawPoint(r - 3, b - 3);
    painter.drawPoint(r - 4, b - 2);
    painter.drawPoint(r - 5, b - 2);

    // mask off which corners we dont want
    QRegion mask;

    mask=QRegion(widget->rect());

    // Remove top-left corner.
    mask -= QRegion(0, 0, 5, 1);
    mask -= QRegion(0, 1, 3, 1);
    mask -= QRegion(0, 2, 2, 1);
    mask -= QRegion(0, 3, 1, 2);
	// Remove top-right corner.
    mask -= QRegion(r - 5, 0, 5, 1);
    mask -= QRegion(r - 3, 1, 3, 1);
    mask -= QRegion(r - 2, 2, 2, 1);
    mask -= QRegion(r - 1, 3, 1, 2);
	// Remove bottom-left corner.
    mask -= QRegion(0, b - 5, 1, 3);
    mask -= QRegion(0, b - 3, 2, 1);
    mask -= QRegion(0, b - 2, 3, 1);
    mask -= QRegion(0, b - 1, 5, 1);
	// Remove bottom-right corner.
    mask -= QRegion(r - 5, b - 1, 5, 1);
    mask -= QRegion(r - 3, b - 2, 3, 1);
    mask -= QRegion(r - 2, b - 3, 2, 1);
    mask -= QRegion(r - 1, b - 5, 1, 2);

    widget->setMask(mask);
}

void fixWmlessPopup(QWidget *widget)
{
    //qDebug("fix_wmless_popup");
    QDesktopWidget *xparent = QApplication::desktop();
    int x = (xparent->width() - widget->width()) / 2;
    if( x < 0 )
	x = 0;
    int y = (xparent->height() - widget->height()) / 2;
    if( y < 0 )
	y = 0;
    widget->move(x, y);
    QCursor::setPos(widget->mapToGlobal(QPoint(widget->width()-5, widget->height()-5)));
}

Qt::Orientation fixOrientation(Qt::Orientation o, Qt::Orientation def)
{
    switch( o )
    {
	case Qt::Horizontal:
	case Qt::Vertical:
	{
	    return o;
	}
	default:
	{
	    // ! need ensure all variants of Orientation
	    if( def == Qt::Vertical || def == Qt::Horizontal )
		return def;
	    else
		return Qt::Vertical;
	}
	
    }
}

Qt::Orientation convertOrientation(const QString& str_value)
{
    if(str_value == "vertical")
	return Qt::Vertical;
    else if(str_value == "horizontal")
	return Qt::Horizontal;
    else
	return (Qt::Orientation)-1;
}

}
