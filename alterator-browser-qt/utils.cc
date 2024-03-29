#include <unistd.h>
#include <pwd.h>

#include <QApplication>
#include <QPainter>
#include <QDesktopWidget>
#include <QFileInfo>
#include <QProcess>
#include <QDesktopServices>
#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
#include <QRandomGenerator>
#endif

#include "messagebox.hh"
#include "browser.hh"

#include "utils.hh"


namespace Utils
{

QString reparentTag(const QString &parent)
{
    QString ret(parent);
    return ret.replace(QStringLiteral(":reparent:"),QString());
}

Qt::Alignment convertAlign(const QString& value)
{
    Qt::Alignment a((Qt::AlignmentFlag)0);
    Q_FOREACH(QString salign, value.split(QLatin1Char(';'), Qt::SkipEmptyParts))
    {
	if 	(QStringLiteral("left") == salign)	a |= Qt::AlignLeft;
	else if (QStringLiteral("right") == salign)	a |= Qt::AlignRight;
	else if (QStringLiteral("middle") == salign)	a |= Qt::AlignVCenter;
	else if (QStringLiteral("hmiddle") == salign)	a |= Qt::AlignHCenter;
	else if (QStringLiteral("center") == salign)	a |= Qt::AlignCenter;
	else if (QStringLiteral("top") == salign)	a |= Qt::AlignTop;
	else if (QStringLiteral("bottom") == salign)	a |= Qt::AlignBottom;
	else
	    qDebug("Unknown align value <%s>", qPrintable(value));
    }
    if( a == 0)
	a = Qt::AlignJustify;
    return a;
}

QLineEdit::EchoMode convertEchoMode(const QString& value)
{
	if (QStringLiteral("no") == value) return QLineEdit::NoEcho;
	else if (QStringLiteral("stars") == value) return QLineEdit::Password;
	return QLineEdit::Normal; //default yes
}

QString simpleQuote(const QString &s)
{
    QString ret(s);
    return ret.replace(QStringLiteral("\\"),QStringLiteral("\\\\")).replace(QStringLiteral("\""),QStringLiteral("\\\""));
}

void widgetCornersRound(QWidget *widget)
{
    int framesize_ = 8;

    QPainter painter(widget);
    QPalette palette = widget->palette();

    QBrush bg_brush = palette.brush( QPalette::Window );
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
    painter.setPen( dark_color );
    frame = widget->rect();
    frame.setHeight( frame.height()-1 );
    frame.setWidth( frame.width()-1 );
    painter.drawRect(frame);

    // set the inner frame
    //painter.setPen( dark_color );
    frame.setRect(frame.x() + framesize_-1, frame.y() + framesize_-1,
	frame.width() - framesize_*2 +2, frame.height() - (framesize_*2) +2);

    // local temp right and bottom
    int r(widget->width());
    int b(widget->height());

    // choose a color from scheme
    painter.setPen( dark_color );

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
    fixWmlessPopupWindow(widget);
    fixWmlessPopupCursor(widget);
}

void fixWmlessPopupWindow(QWidget *widget)
{
    QDesktopWidget *xparent = QApplication::desktop();
    int x = (xparent->width() - widget->width()) / 2;
    if( x < 0 )
	x = 0;
    int y = (xparent->height() - widget->height()) / 2;
    if( y < 0 )
	y = 0;
    widget->move(x, y);
    fixWmlessPopupCursor(widget);
}

void fixWmlessPopupCursor(QWidget *widget)
{
#if 0
    QCursor::setPos(widget->mapToGlobal(QPoint(5,5)));
#else
    QCursor::setPos(widget->mapToGlobal(QPoint(widget->width()-5, widget->height()-5)));
#endif
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
	    switch( def )
	    {
		case Qt::Horizontal:
		case Qt::Vertical:
		    return def;
		default:
		    return Qt::Vertical;
	    }
	}
    }
}

Qt::Orientation convertOrientation(const QString& str_value)
{
    if(str_value == QStringLiteral("vertical"))
	return Qt::Vertical;
    else if(str_value == QStringLiteral("horizontal"))
	return Qt::Horizontal;
    else
	return (Qt::Orientation)0;
}

void openUrl(const QUrl &url)
{
	int loginuid = 0;
	if( geteuid() == 0 )
	{
	    QString proc_path(QStringLiteral("/proc/self/loginuid"));
	    if( QFileInfo(proc_path).exists() )
	    {
		QFile proc_file(proc_path);
		QString proc_content;
		if( proc_file.open(QIODevice::ReadOnly) )
		    proc_content = QLatin1String(proc_file.readLine().trimmed());
		if( !proc_content.isEmpty() )
		{
		    bool ok;
		    loginuid = proc_content.toInt(&ok, 10);
		    if( !ok )
			loginuid = 0;
		}
	    }
	}
	if( loginuid > 0 )
	{
	    struct passwd *pwd = getpwuid(loginuid);
	    QProcess url_exec;
	    url_exec.setProgram(QStringLiteral("su"));
	    url_exec.setArguments(QStringList() << QStringLiteral("-l") << QStringLiteral("-c") << QString(QStringLiteral("xdg-open \'")).append(url.toString()).append(QStringLiteral("\'")) << QLatin1String(pwd->pw_name));
	    url_exec.setStandardOutputFile(QProcess::nullDevice(), QIODevice::Append);
	    url_exec.startDetached();
	} else {
	    QDesktopServices::openUrl(url);
	}
}

QString getRandomString(int sz)
{
    const QByteArray dict("0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz");

    QString result;
    int index = 0;
    for( int i = 0; i < sz; i++ ) {
#if QT_VERSION >= QT_VERSION_CHECK(5,10,0)
	index = QRandomGenerator::global()->generate() % dict.size();
#else
	index = qrand() % dict.size();
#endif
	result.append(QLatin1Char(dict.at(index)));
    }
    return result;
}
}
