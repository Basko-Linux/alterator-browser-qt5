#include <QHBoxLayout>

#include "splashscreen.hh"
#include "utils.hh"

SplashScreen::SplashScreen(QWidget *parent):
    Popup(parent)
{
    setObjectName("splash_screen");

    QHBoxLayout *lay = new QHBoxLayout(view());
    lay->setMargin(0);
    QLayout *l = layout();
    if(l)
	l->setMargin(0);

    label = new QLineEdit(this);
#if 0
    label->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
#endif
    label->setAlignment(Qt::AlignCenter);
    label->setFrame(false);
    label->setReadOnly(true);
    label->setDragEnabled(false);
    lay->addWidget(label);

#if 0
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    move(parent->geometry().center() - geometry().center());
    raise();
#endif
    show();
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::setText(const QString &txt)
{
    label->setText(txt);
    QFontMetrics m(label->font());
    setMinimumWidth(m.boundingRect(txt).width() + 25);
#if 0
    QRect pg(parentWidget()->geometry());
    int w = width();
    int h = height();
    setGeometry((pg.width()-w)/2, (pg.height()-h)/2,w ,h);
#endif
}
