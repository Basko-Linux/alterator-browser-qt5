
#include "al_dateedit.hh"

ADateEdit::ADateEdit(QWidget *parent, const Qt::Orientation):
    AWidget<QWidget>(parent)
{
    lay = new QVBoxLayout(this);
    lay->setContentsMargins(0,0,0,0);
    lay->setSpacing(0);

    calendar = new QCalendarWidget(this);
    calendar->setVisible(false);
    calendar->setGridVisible(true);
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    calendar->setFirstDayOfWeek( Qt::Monday ); // FIXME

    date_edit = new QDateEdit(this);
    //date_edit->setSizePolicy(QSizePolicy::Maximum, date_edit->sizePolicy().verticalPolicy());
    date_edit->setDate(calendar->selectedDate());
    date_edit->setCalendarPopup( true );

    lay->addWidget(calendar, 0, Qt::AlignCenter);
    lay->addStretch(1);
    lay->addWidget(date_edit, 0, Qt::AlignCenter);

    connect(calendar, SIGNAL(activated(const QDate&)), date_edit, SLOT(setDate(const QDate&)));
    connect(calendar, SIGNAL(clicked(const QDate&)), date_edit, SLOT(setDate(const QDate&)));
    connect(calendar, SIGNAL(selectionChanged()), this, SIGNAL(changed()));
    connect(date_edit, SIGNAL(dateChanged(const QDate&)), calendar, SLOT(setSelectedDate(const QDate&)));

    setFocusProxy(date_edit);
}

ADateEdit::~ADateEdit()
{}

void ADateEdit::setDate(const QString& new_date)
{
    date_edit->setDate(QDate::fromString(new_date, Qt::ISODate));
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

alDateEdit::alDateEdit(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
    alWidgetPre<ADateEdit>(attr,WDateEdit, id, parent)
{}

void alDateEdit::setAttr(const QString& name,const QString& value)
{
    if (QStringLiteral("text") == name)
        wnd_->setDate(value);
    else if (QStringLiteral("value") == name)
        wnd_->setDate(value);
    else if (QStringLiteral("expanded") == name)
        wnd_->setExpanded(value == QStringLiteral("true"));
    else
        alWidget::setAttr(name,value);
}

void alDateEdit::registerEvent(const QString& name)
{
    if( QStringLiteral("changed") == name )
        connect(wnd_,SIGNAL( changed() ),SLOT(onChange()));
}


QString alDateEdit::postData() const
{
    QString ret;
    ret.append(QString(QStringLiteral(" (text . \"%1\" )")).arg(wnd_->date()));
    ret.append(QString(QStringLiteral(" (value . \"%1\" )")).arg(wnd_->date()));
    return ret;
}
