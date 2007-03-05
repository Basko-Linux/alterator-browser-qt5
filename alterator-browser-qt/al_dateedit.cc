
#include "al_dateedit.hh"

ADateEdit::ADateEdit(QWidget *parent):
    QWidget(parent)
{
    lay = new QVBoxLayout(this);
    lay->setMargin(0);
    lay->setSpacing(0);

    calendar = new QCalendarWidget(this);
    calendar->setVisible(false);
    calendar->setGridVisible(true);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendar->setFirstDayOfWeek( Qt::Monday ); // FIXME

    date_edit = new QDateEdit(this);
    date_edit->setDate(calendar->selectedDate());
    date_edit->setCalendarPopup( true );

    lay->addWidget(calendar);
    lay->addWidget(date_edit);

    connect(calendar, SIGNAL(activated(const QDate&)), date_edit, SLOT(setDate(const QDate&)));
    connect(calendar, SIGNAL(clicked(const QDate&)), date_edit, SLOT(setDate(const QDate&)));
    connect(date_edit, SIGNAL(dateChanged(const QDate&)), calendar, SLOT(setSelectedDate(const QDate&)));

    connect(calendar, SIGNAL(editingFinished()), this, SIGNAL(changed()));
}

ADateEdit::~ADateEdit()
{}

void ADateEdit::setDate(const QString& new_date)
{
    QVariant x(new_date);
    QDate xdate = x.toDate();
    calendar->setSelectedDate(xdate);
    date_edit->setDate(xdate);
}

QString ADateEdit::date()
{
    return date_edit->date().toString(Qt::ISODate);
}

void ADateEdit::setExpanded(bool expand)
{
    calendar->setVisible(expand);
    date_edit->setCalendarPopup(!expand);
    lay->setSpacing(expand?5:0);
}

// alDateEdit

alDateEdit::alDateEdit(const QString& id,const QString& parent):
    alWidgetPre<ADateEdit>(DateEdit, id, parent)
{}

void alDateEdit::setAttr(const QString& name,const QString& value)
{
    if ("text" == name)
        wnd_->setDate(value);
    else if ("expanded" == name)
        wnd_->setExpanded("false" == value);
    else
        alWidget::setAttr(name,value);
}

void alDateEdit::registerEvent(const QString& name)
{
    if( "changed" == name )
        connect(wnd_,SIGNAL( changed() ),SLOT(onChange()));
}


QString alDateEdit::postData() const
{
	return QString(" (text . \"%1\" )").arg(wnd_->date());
}
