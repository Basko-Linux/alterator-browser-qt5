
#include <QPainter>
#include <QTimerEvent>
#include <QEvent>

#include "al_timeedit.hh"

// TimeEditFocusEventHandler

TimeEditFocusEventHandler::TimeEditFocusEventHandler(QObject *parent):
    QObject(parent)
{
}

TimeEditFocusEventHandler::~TimeEditFocusEventHandler()
{}

bool TimeEditFocusEventHandler::eventFilter(QObject *obj, QEvent *e)
{
    switch(e->type())
    {
	case QEvent::FocusIn:
	{
	    emit focusIn();
	    break;
	}
	case QEvent::FocusOut:
	{
	    emit focusOut();
	    break;
	}
	default:
	    break;
    }
    return QObject::eventFilter(obj, e);
}

// AnalogClock

AnalogClock::AnalogClock(QWidget *parent):
    QWidget(parent)
{
    setMinimumSize(64, 64);

    offset = 0;
    last_time = QTime::currentTime();
    tmr_id = 0;
    setMinimumSize(128,128);
    setup();
    start();
}

AnalogClock::~AnalogClock()
{}

void AnalogClock::setup()
{
    QPalette::ColorGroup group = isEnabled()? QPalette::Active: QPalette::Disabled;
    QPalette pal = palette();
    bg_color = pal.color(group, QPalette::Base);
    round_pen = QPen(pal.dark().color());
    hou_mrk_pen = QPen(pal.color(group, QPalette::Text));
    min_mrk_pen = QPen(pal.color(group, QPalette::Text));
    hou_pen = QPen(pal.color(group, QPalette::Text));
    min_pen = QPen(pal.color(group, QPalette::Text));
    sec_pen = QPen(QColor(isEnabled()? "red": "brown"));

    round_pen.setWidth(6);
    hou_mrk_pen.setWidth(4); hou_mrk_pen.setCapStyle(Qt::RoundCap);
    min_mrk_pen.setWidth(2); min_mrk_pen.setCapStyle(Qt::RoundCap);
    hou_pen.setWidth(4); hou_pen.setCapStyle(Qt::RoundCap);
    min_pen.setWidth(2); min_pen.setCapStyle(Qt::RoundCap);
    sec_pen.setWidth(1);
    clock_width = width();
    clock_height = height();
}

void AnalogClock::setTime(const QTime& new_time)
{
    if(tmr_id <= 0)
    {
	offset = last_time.secsTo(new_time);
	update();
    }
    else
	offset = QTime::currentTime().secsTo(new_time);
}

void AnalogClock::start()
{
    if( tmr_id > 0 )
	return;
    tmr_id = startTimer(1000);
}

void AnalogClock::stop()
{
    if( tmr_id > 0 )
	killTimer(tmr_id);
    else
	last_time = QTime::currentTime();
    tmr_id = 0;
}

void AnalogClock::reset()
{
    offset = 0;
}

bool AnalogClock::event(QEvent* e)
{
    switch( e->type() )
    {
        case QEvent::PaletteChange:
	case QEvent::EnabledChange:
	{
	    setup();
	    break;
	}
	default:
	    break;
    }
    return QWidget::event(e);
}

void AnalogClock::resizeEvent(QResizeEvent*)
{
    setup();
}

void AnalogClock::timerEvent(QTimerEvent* e)
{
    if( e->timerId() == tmr_id )
	update();
}

void AnalogClock::paintEvent(QPaintEvent*)
{
	QTime tm;
	if(tmr_id > 0)
	    tm = QTime::currentTime().addSecs(offset);
	else
	    tm = last_time.addSecs(offset);
	int h = tm.hour();
	int m = tm.minute();
	int s = tm.second();

	QPainter p(this);
	p.setRenderHints(QPainter::Antialiasing);

	int round_pen_width = round_pen.width();
	QPainterPath bg;
	bg.addEllipse(round_pen_width,round_pen_width,clock_width-round_pen_width*2,clock_height-round_pen_width*2);
	p.fillPath(bg, bg_color);
	p.setPen(round_pen);
	p.drawEllipse(round_pen_width/2,round_pen_width/2,clock_width-round_pen_width,clock_height-round_pen_width);

	p.translate(clock_width/2, clock_height/2);

	qreal deg;

	// marks
	p.setPen(hou_mrk_pen);
	int hou_mark_y = clock_width*0.5-round_pen_width-3;
	for (int i=0;i<12;i++){
	    deg = 30*i;
	    p.rotate(deg);
	    p.drawLine(0, hou_mark_y, 0, hou_mark_y-min_mrk_pen.width()*3);
	    p.rotate(-deg);
	}
	p.setPen(min_mrk_pen);
	int min_mark_y = clock_width*0.5-round_pen_width-3;
	for (int i=0;i<72;i++){
	    deg = 6*i;
	    p.rotate(deg);
	    p.drawLine(0, min_mark_y, 0, min_mark_y-1);
	    p.rotate(-deg);
	}

	// hours
	deg = 30 * h + m/2;
	p.rotate(deg);
	p.setPen(hou_pen);
	p.drawLine(0, 0, 0, -(clock_width*0.3));
	p.rotate(-deg);

	// minutes
	deg = 6 * m;
	p.rotate(deg);
	p.setPen(min_pen);
	p.drawLine(0, 0, 0, -(clock_width*0.39));
	p.rotate(-deg);

	// seconds
	QPainterPath sec;
	sec.addRect(0,10,0,-(min_mark_y+7));
	sec.addEllipse(-3,-3,6,6);
	sec.addEllipse(-5,-(min_mark_y-hou_mrk_pen.width()*3+2),10,10);
	QPainterPath rounds;
	rounds.addEllipse(-2,-2,4,4);
	rounds.addEllipse(-4,-(min_mark_y-hou_mrk_pen.width()*3+1),8,8);

	deg = 6 * s;
	p.rotate(deg);
	p.setPen(sec_pen);
	p.drawPath(sec);
	p.fillPath(sec,sec_pen.color());
	p.fillPath(rounds,bg_color);
	p.rotate(-deg);
}

// ATimeEdit

ATimeEdit::ATimeEdit(QWidget *parent):
    QWidget(parent)
{
    state_edit = false;
    offset = 0;
    tmr_id = 0;
    setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);

    lay = new QVBoxLayout(this);
    lay->setMargin(0);
    lay->setSpacing(0);

    clock = new AnalogClock(this);
    clock->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    clock->hide();

    time_edit = new QTimeEdit(this);
    //time_edit->setSizePolicy(QSizePolicy::MinimumExpanding,QSizePolicy::MinimumExpanding);
    time_edit->setTime(QTime::currentTime());

    lay->addStretch(1);
    lay->addWidget(clock, 0, Qt::AlignCenter);
    lay->addStretch(1);
    lay->addWidget(time_edit, 0, Qt::AlignCenter);

    time_edit_focus = new TimeEditFocusEventHandler(this);
    time_edit->installEventFilter(time_edit_focus);

    connect(time_edit, SIGNAL(timeChanged(const QTime&)), this, SLOT(onChange(const QTime&)));
    connect(time_edit, SIGNAL(editingFinished()), this, SIGNAL(changed()));
    connect(time_edit_focus, SIGNAL(focusIn()), this, SLOT(onFocusIn()));
    connect(time_edit_focus, SIGNAL(focusOut()), this, SLOT(onFocusOut()));

    start();
}

ATimeEdit::~ATimeEdit()
{}

void ATimeEdit::start()
{
    clock->start();
    if( tmr_id > 0 )
	return;
    tmr_id = startTimer(1000);
}

void ATimeEdit::stop()
{
    clock->stop();
    if( tmr_id > 0 )
	killTimer(tmr_id);
    tmr_id = 0;
}

void ATimeEdit::reset()
{
    offset = 0;
    clock->reset();
}

void ATimeEdit::timerEvent(QTimerEvent* e)
{
    if( e->timerId() == tmr_id && !state_edit )
	showTime();
}

void ATimeEdit::setTime(const QString& new_time)
{
    bool state_edit_old = state_edit;
    state_edit = true;
    time_edit->setTime(QTime::fromString(new_time, Qt::ISODate));
    state_edit = state_edit_old;
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
    if(state_edit)
    {
	offset = QTime::currentTime().secsTo(new_time);
	clock->setTime(new_time);
    }
}

void ATimeEdit::onFocusIn()
{
    state_edit = true;
    stop();
    onChange(time_edit->time());
}

void ATimeEdit::onFocusOut()
{
    start();
    onChange(time_edit->time());
    state_edit = false;
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
    if ("reset" == name)
        wnd_->reset();
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
