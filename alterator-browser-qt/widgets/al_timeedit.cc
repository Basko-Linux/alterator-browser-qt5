
#include <QPainter>
#include <QTimerEvent>

#include "al_timeedit.hh"

AAnalogClock::AAnalogClock(QWidget *parent):
    QWidget(parent)
{
    setMinimumSize(64, 64);

    offset = 0;
    tmr_id = 0;
    bg = QPixmap(":/images/clock.png");
    setFixedSize(bg.width(), bg.height());
    setupColors();
    setupSize();
    start();
}

AAnalogClock::~AAnalogClock()
{}

void AAnalogClock::setupColors()
{
    QPalette pal = palette();
    //hou_pen = QPen(pal.dark().color());
    hou_pen = QPen(QColor("black"));
    //min_pen = QPen(pal.dark().color());
    min_pen = QPen(QColor("black"));
    sec_pen = QPen(QColor("red"));
}

void AAnalogClock::setupSize()
{
    hou_pen.setWidth(4); hou_pen.setCapStyle(Qt::RoundCap);
    min_pen.setWidth(2); min_pen.setCapStyle(Qt::RoundCap);
    sec_pen.setWidth(1);
    clock_width = width();
    clock_height = height();
}

void AAnalogClock::setOffset(int new_offset)
{
    offset = new_offset;
}

void AAnalogClock::start()
{
    offset = 0;
    if( tmr_id > 0 )
	stop();
    tmr_id = startTimer(1000);
}

void AAnalogClock::stop()
{
    if( tmr_id > 0 )
	killTimer(tmr_id);
    tmr_id = 0;
}

bool AAnalogClock::event(QEvent* e)
{
    if( e->type() == QEvent::PaletteChange )
    {
	setupColors();
    }
    return QWidget::event(e);
}

void AAnalogClock::resizeEvent(QResizeEvent*)
{
    setupSize();
}

void AAnalogClock::timerEvent(QTimerEvent* e)
{
    if( e->timerId() == tmr_id )
	update();
}

void AAnalogClock::paintEvent(QPaintEvent*)
{
	QTime tm = (QTime::currentTime()).addSecs(offset);
	int h = tm.hour();
	int m = tm.minute();
	int s = tm.second();

	QPainter p(this);
	p.setRenderHints(QPainter::Antialiasing);

	p.drawPixmap(QPoint(0,0), bg);
	p.translate(clock_width/2, clock_height/2);

	qreal deg;
	deg = 30 * h + m/2;
	p.rotate(deg);
	p.setPen(hou_pen);	p.drawLine(0, 0, 0, -(clock_width*0.3));
	p.rotate(-deg);

	deg = 6 * m;
	p.rotate(deg);
	p.setPen(min_pen);	p.drawLine(0, 0, 0, -(clock_width*0.40));
	p.rotate(-deg);

	deg = 6 * s;
	p.rotate(deg);
	p.setPen(sec_pen);	p.drawLine(0, 0, 0, -(clock_width*0.40));
	p.rotate(-deg);
}

// ATimeEdit

ATimeEdit::ATimeEdit(QWidget *parent):
    QWidget(parent)
{
    offset = 0;
    tmr_id = 0;

    lay = new QVBoxLayout(this);
    lay->setMargin(0);
    lay->setSpacing(0);

    clock = new AAnalogClock(this);
    clock->hide();

    time_edit = new QTimeEdit(this);
    //time_edit->setSizePolicy(QSizePolicy::Maximum, time_edit->sizePolicy().verticalPolicy());
    time_edit->setTime(QTime::currentTime());

    lay->addWidget(clock, 0, Qt::AlignCenter);
    lay->addStretch(1);
    lay->addWidget(time_edit, 0, Qt::AlignCenter);

    connect(time_edit, SIGNAL(timeChanged(const QTime&)), this, SLOT(onChange(const QTime&)));
    connect(time_edit, SIGNAL(editingFinished()), this, SIGNAL(changed()));

    start();
}

ATimeEdit::~ATimeEdit()
{}

void ATimeEdit::start()
{
    offset = 0;
    if( tmr_id > 0 )
	stop();
    tmr_id = startTimer(1000);
    clock->start();
}

void ATimeEdit::stop()
{
    clock->stop();
    if( tmr_id > 0 )
	killTimer(tmr_id);
    tmr_id = 0;
}

void ATimeEdit::timerEvent(QTimerEvent* e)
{
    if( e->timerId() == tmr_id )
	showTime();
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
    clock->setOffset(offset);
}

void ATimeEdit::showTime()
{
    time_edit->setTime(QTime::currentTime().addSecs(offset));
}

// alTimeEdit

alTimeEdit::alTimeEdit(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
    alWidgetPre<ATimeEdit>(attr,WTimeEdit, id, parent)
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
