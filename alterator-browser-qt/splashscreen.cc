#include <QGridLayout>
#include <QPaintEvent>

#include "splashscreen.hh"
#include "utils.hh"
#include "main_window.hh"

extern MainWindow *main_window;

SplashScreen::SplashScreen(QWidget *parent):
    QWidget(parent, Qt::FramelessWindowHint|Qt::CustomizeWindowHint|Qt::Window)
{
    if( !main_window->haveWindowManager() )
	setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);
    setWindowModality(Qt::ApplicationModal);
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    QGridLayout *lay = new QGridLayout(this);
    lay->setMargin(10);

    label = new QLabel(this);
    label->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    label->setAlignment(Qt::AlignCenter);
    lay->addWidget(label, 0, 0, Qt::AlignCenter);
}

SplashScreen::~SplashScreen()
{
}

void SplashScreen::setText(const QString &txt)
{
    label->setText(txt);
}

void SplashScreen::paintEvent(QPaintEvent* e)
{
//    if( !main_window->haveWindowManager() )
//    {
	Utils::widgetCornersRound(this);
//    }
}
