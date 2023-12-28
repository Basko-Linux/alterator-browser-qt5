#include <QHBoxLayout>

#include "splashscreen.hh"
#include "utils.hh"

SplashScreen::SplashScreen(QWidget *parent):
    Popup(parent)
{
    setObjectName(QStringLiteral("splash_screen"));

    setFrameStyle(QFrame::NoFrame);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_TranslucentBackground);

    QHBoxLayout *lay = new QHBoxLayout(view());
    lay->setContentsMargins(0,0,0,0);
    QLayout *l = layout();
    if(l)
	l->setContentsMargins(0,0,0,0);

    label = new QLineEdit(this);
    label->setAlignment(Qt::AlignCenter);
    label->setFrame(false);
    label->setReadOnly(true);
    label->setDragEnabled(false);
    lay->addWidget(label);

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
}
