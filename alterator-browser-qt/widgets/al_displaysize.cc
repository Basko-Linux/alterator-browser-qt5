#include "al_displaysize.hh"

#include <QGuiApplication>
#ifdef HAVE_X11
#include <X11/Xlib.h>
#include <QX11Info>
#endif

alDisplaySize::alDisplaySize(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
	alWidgetPre<ADisplaySize>(attr,WDisplaySize,id,parent)
{
    QString info_text(tr("unknown"));
    QString platform_name(qApp->platformName());
#ifdef HAVE_X11
    if( platform_name == QStringLiteral("xcb") ) {
	Display *dpy = QX11Info::display();
	int scr = QX11Info::appScreen();
	info_text = QString(QStringLiteral("%1%2%3")).arg(XDisplayWidthMM(dpy,scr)).arg(QString::fromUtf8("×")).arg(XDisplayHeightMM(dpy,scr));
    }
#endif
    wnd_->setText(info_text);
}

void alDisplaySize::setAttr(const QString& name,const QString& value)
{
	alWidget::setAttr(name,value);
}
