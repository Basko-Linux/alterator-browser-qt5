#include "al_displaysize.hh"

#ifdef Q_WS_X11
#include <X11/Xlib.h>
#include <QX11Info>
#endif

alDisplaySize::alDisplaySize(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
	alWidgetPre<ADisplaySize>(attr,WDisplaySize,id,parent)
{
    QString info_text(tr("unknown"));
#ifdef Q_WS_X11
    QX11Info x11info;
    Display *dpy = x11info.display();
    int scr = x11info.screen();
    info_text = QString("%1%2%3").arg(XDisplayWidthMM(dpy,scr)).arg(QString::fromUtf8("×")).arg(XDisplayHeightMM(dpy,scr));
#endif
    wnd_->setText(info_text);
}

void alDisplaySize::setAttr(const QString& name,const QString& value)
{
	alWidget::setAttr(name,value);
}
