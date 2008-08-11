#include <QHBoxLayout>

#include "splashscreen.hh"
#include "utils.hh"
#include "main_window.hh"

SplashScreen::SplashScreen(QWidget *parent):
    QFrame(parent)
{
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);

    QHBoxLayout *lay = new QHBoxLayout(this);
    lay->setMargin(0);

    label = new QLineEdit(this);
    label->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    label->setAlignment(Qt::AlignCenter);
    label->setFrame(false);
    label->setReadOnly(true);
    label->setDragEnabled(false);
    lay->addWidget(label);

    move(parent->geometry().center() - geometry().center());
    raise();
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
    QRect pg(parentWidget()->geometry());
    int w = width();
    int h = height();
    setGeometry((pg.width()-w)/2, (pg.height()-h)/2,w ,h);
}
