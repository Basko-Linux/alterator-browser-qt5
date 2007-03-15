
#include <QPainter>

#include "al_timeedit.hh"

AAnalogClock::AAnalogClock(QWidget *parent):
    QWidget(parent)
{
    setMinimumSize(64, 64);

    offset = 0;
    deg_per_hou = 360/12;
    deg_per_min = 360/60;
    deg_per_sec = 360/60;
    bg = QPixmap(":/images/clock.png");
    setFixedSize(bg.width(), bg.height());
    hpen = QPen(QColor("black")); hpen.setWidth(4); hpen.setCapStyle(Qt::RoundCap);
    mpen = QPen(QColor("black")); mpen.setWidth(2); mpen.setCapStyle(Qt::RoundCap);
    spen = QPen(QColor("red"));   spen.setWidth(1);

    tmr = new QTimer(this);
    tmr->setInterval(1000);
    connect(tmr, SIGNAL(timeout()), this, SLOT(repaint()));
    start();
}

AAnalogClock::~AAnalogClock()
{}

void AAnalogClock::setOffcet(int new_offcet)
{
    offset = new_offcet;
}

void AAnalogClock::start()
{
    offset = 0;
    tmr->start();
}

void AAnalogClock::stop()
{
    tmr->stop();
}

void AAnalogClock::paintEvent(QPaintEvent*)
{
	QTime tm = (QTime::currentTime()).addSecs(offset);
	int h = tm.hour();
	int m = tm.minute();
	int s = tm.second();

	QPainter p(this);
	p.setRenderHints(QPainter::Antialiasing);

	int wdth = width();
	int hght = height();

	p.drawPixmap(QPoint(0,0), bg);
	p.translate(wdth/2, hght/2);

	double deg;
	deg = deg_per_hou * h;
	p.rotate(deg);
	p.setPen(hpen);	p.drawLine(0, 0, 0, -(wdth*0.3));
	p.rotate(-deg);

	deg = deg_per_min * m;
	p.rotate(deg);
	p.setPen(mpen);	p.drawLine(0, 0, 0, -(wdth*0.40));
	p.rotate(-deg);

	deg = deg_per_sec * s;
	p.rotate(deg);
	p.setPen(spen);	p.drawLine(0, 0, 0, -(wdth*0.40));
	p.rotate(-deg);
}

// ATimeEdit

ATimeEdit::ATimeEdit(QWidget *parent):
    QWidget(parent)
{
    offset = 0;

    lay = new QGridLayout(this);
    lay->setMargin(0);
    lay->setSpacing(0);

    clock = new AAnalogClock(this);
    clock->hide();

    time_edit = new QTimeEdit(this);
    //time_edit->setSizePolicy(QSizePolicy::Maximum, time_edit->sizePolicy().verticalPolicy());
    time_edit->setTime(QTime::currentTime());

    lay->addWidget(clock, 0, 0, Qt::AlignCenter);
    lay->addWidget(time_edit, 1, 0, Qt::AlignCenter);

    tmr = new QTimer(this);
    tmr->setInterval(1000);
    connect(tmr, SIGNAL(timeout()), this, SLOT(showTime()));
    connect(time_edit, SIGNAL(timeChanged(const QTime&)), this, SLOT(onChange(const QTime&)));
    connect(time_edit, SIGNAL(editingFinished()), this, SIGNAL(changed()));

    start();
}

ATimeEdit::~ATimeEdit()
{}

void ATimeEdit::start()
{
    offset = 0;
    tmr->start();
    clock->start();
}

void ATimeEdit::stop()
{
    clock->stop();
    tmr->stop();
}

void ATimeEdit::setTime(const QString& new_time)
{
    time_edit->setTime(QTime::fromString(new_time, Qt::ISODate));
}

QString ATimeEdit::time()
{
    return time_edit->time().toString(Qt::ISODate);
}

void ATimeEdit::setExpanded(bool expand)
{
    if(expand)
	clock->show();
    else
	clock->hide();
    lay->setSpacing(expand?5:0);
}

void ATimeEdit::onChange(const QTime& new_time)
{
    offset = QTime::currentTime().secsTo(new_time);
    clock->setOffcet(offset);
}

void ATimeEdit::showTime()
{
    time_edit->setTime(QTime::currentTime().addSecs(offset));
}

// alTimeEdit

alTimeEdit::alTimeEdit(const QString& id,const QString& parent):
    alWidgetPre<ATimeEdit>(TimeEdit, id, parent)
{}

void alTimeEdit::setAttr(const QString& name,const QString& value)
{
    if ("text" == name)
        wnd_->setTime(value);
    if ("start" == name)
        wnd_->start();
    if ("stop" == name)
        wnd_->stop();
    else if ("expanded" == name)
        wnd_->setExpanded("true" == value);
    else
        alWidget::setAttr(name,value);
}

void alTimeEdit::registerEvent(const QString& name)
{
    if( "changed" == name )
        connect(wnd_,SIGNAL( changed() ),SLOT(onChange()));
}


QString alTimeEdit::postData() const
{
	return QString(" (text . \"%1\" )").arg(wnd_->time());
}
