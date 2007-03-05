
#include "al_timeedit.hh"

ADigitalClock::ADigitalClock(QWidget *parent)
    : QLCDNumber(parent)
{
    own_time = QTime::currentTime();
    own_time.start();

    setSegmentStyle(Filled);

    showTime();

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));

    timer->start(1000);
}

ADigitalClock::~ADigitalClock()
{}

void ADigitalClock::showTime()
{
    QString text = own_time.toString("hh:mm:ss");
    if( (own_time.second() % 2) == 0 )
    {
        text[2] = ' ';
        text[5] = ' ';
    }
    display(text);
}

void ADigitalClock::setTime(const QTime& new_time)
{
    own_time = new_time;
}

// ATimeEdit

ATimeEdit::ATimeEdit(QWidget *parent):
    QWidget(parent)
{
    own_time = QTime::currentTime();
    own_time.start();

    lay = new QVBoxLayout(this);
    lay->setMargin(0);
    lay->setSpacing(0);

    clock = new ADigitalClock(this);
    clock->setVisible(false);

    time_edit = new QTimeEdit(this);
    time_edit->setTime(own_time);

    lay->addWidget(clock);
    lay->addWidget(time_edit);

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    connect(time_edit, SIGNAL(editingFinished()), this, SIGNAL(onChange()));

    timer->start(1000);
}

ATimeEdit::~ATimeEdit()
{}

void ATimeEdit::setTime(const QString& new_time)
{
    QVariant x(new_time);
    QTime xtime = x.toTime();
    clock->setTime(xtime);
    time_edit->setTime(xtime);
}

QString ATimeEdit::time()
{
    return time_edit->time().toString(Qt::ISODate);
}

void ATimeEdit::setExpanded(bool expand)
{
    clock->setVisible(expand);
    lay->setSpacing(expand?5:0);
}

void ATimeEdit::onChange()
{
    clock->setTime(time_edit->time());
    emit changed();
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
        wnd_->setExpanded("false" == value);
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
