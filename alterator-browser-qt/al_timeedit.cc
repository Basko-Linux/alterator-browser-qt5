
#include <QPainter>

#include "al_timeedit.hh"

AAnalogClock::AAnalogClock(QWidget *parent):
    QWidget(parent)
{
    offset = 0;
    deg_per_hou = 360 / 12;
    deg_per_min = 360 / 60;
    deg_per_sec = 360 / 60;
    hpen = QPen(QColor("black")); hpen.setWidth(4); hpen.setCapStyle(Qt::RoundCap);
    mpen = QPen(QColor("black")); mpen.setWidth(2); mpen.setCapStyle(Qt::RoundCap);
    spen = QPen(QColor("red"));   spen.setWidth(1);

    QTimer *timer = new QTimer(this);
    timer->setInterval(1000);
    connect(timer, SIGNAL(timeout()), this, SLOT(repaint()));
    timer->start();
}

AAnalogClock::~AAnalogClock()
{}

void AAnalogClock::setOffcet(int new_offcet)
{
    offset = new_offcet;
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
	int hpen_w = hpen.width();

	int round;
	if( wdth > hght  )
	    round = hght-hpen_w;
	else
	    round = wdth-hpen_w;

	int round_x, round_y;
	round_x = round_y = -round/2;

	p.translate(wdth/2, hght/2);

	p.setPen(hpen);
	p.drawEllipse(QRect(round_x, round_y, round, round));

	double deg;
	// hours
	deg = deg_per_hou * h;
	p.rotate(deg);
	p.setPen(hpen);
	p.drawLine(0, 0, 0, -(round*0.3));
	p.rotate(-deg);

	// minutes
	deg = deg_per_min * m;
	p.rotate(deg);
	p.setPen(mpen);
	p.drawLine(0, 0, 0, -(round*0.45));
	p.rotate(-deg);

	// seconds
	deg = deg_per_sec * s;
	p.rotate(deg);
	p.setPen(spen);
	p.drawLine(0, 0, 0, -(round*0.45));
	p.rotate(-deg);
}

// ATimeEdit

ATimeEdit::ATimeEdit(QWidget *parent):
    QWidget(parent)
{
    offset = 0;

    lay = new QVBoxLayout(this);
    lay->setMargin(0);
    lay->setSpacing(0);

    clock = new AAnalogClock(this);
    clock->hide();

    time_edit = new QTimeEdit(this);
    time_edit->setTime(QTime::currentTime());

    lay->addWidget(clock);
    lay->addWidget(time_edit);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    connect(time_edit, SIGNAL(timeChanged(const QTime&)), this, SLOT(onChange(const QTime&)));
    connect(time_edit, SIGNAL(editingFinished()), this, SIGNAL(changed()));

    timer->start(1000);
}

ATimeEdit::~ATimeEdit()
{}

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
