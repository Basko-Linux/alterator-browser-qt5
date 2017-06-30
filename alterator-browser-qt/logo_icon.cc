
#include <QMouseEvent>

#include "logo_icon.hh"

LogoIcon::LogoIcon(QWidget *parent, bool clickable_):
    QLabel(parent)
{
    clickable = clickable_;
    if( clickable )
	setCursor(Qt::PointingHandCursor);
}

LogoIcon::~LogoIcon()
{}

void LogoIcon::mousePressEvent(QMouseEvent *e)
{
    if( clickable )
    {
	e->accept();
	Q_EMIT clicked();
    }
}
