
#include "al_timeedit.hh"

ADigitalClock::ADigitalClock(QWidget *parent)
    : QLCDNumber(parent)
{
    setNumDigits( 8 );
    setSegmentStyle(Filled);

    setTime(QTime::currentTime());
}

ADigitalClock::~ADigitalClock()
{}

void ADigitalClock::setTime(const QTime& new_time)
{
    QString text = new_time.toString("hh:mm:ss");
    if( (new_time.second() % 2) == 0 )
    {
        text[5] = ' ';
    }
    display(text);
}

// ATimeEdit

ATimeEdit::ATimeEdit(QWidget *parent):
    QWidget(parent)
{
    offset = 0;

    lay = new QVBoxLayout(this);
    lay->setMargin(0);
    lay->setSpacing(0);

    clock = new ADigitalClock(this);
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
    clock->setTime(new_time);
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
