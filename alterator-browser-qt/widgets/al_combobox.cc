#include <QApplication>

#include "al_combobox.hh"
#include "a_pixmaps.hh"
#include "printfilter.hh"

AComboBox::AComboBox(QWidget *parent, const Qt::Orientation):
    AWidget<QComboBox>(parent)
{
    text_changed_ = false;
    view()->setSizePolicy(view()->sizePolicy().horizontalPolicy(), QSizePolicy::MinimumExpanding);

    connect(this, SIGNAL(editTextChanged(const QString&)),
	this, SLOT(onTextChange(const QString&)));
    connect(this, SIGNAL(activated(const QString&)),
	this, SLOT(onActivate(const QString&)));

    view()->installEventFilter(new PrintFilter(this));
}

AComboBox::~AComboBox() {}

void AComboBox::keyPressEvent(QKeyEvent *e)
{
    switch( e->key() )
    {
	case Qt::Key_Enter:
	case Qt::Key_Return:
	{
	    Q_EMIT editingFinished();
	    break;
	}
	default:
	    QComboBox::keyPressEvent(e);
    }
}

void AComboBox::focusOutEvent(QFocusEvent* e)
{
    Qt::FocusReason reason = e->reason();
    if( reason != Qt::PopupFocusReason
        && !(QApplication::activePopupWidget() && QApplication::activePopupWidget()->parentWidget() == this))
    {
	if( text_changed_ )
	    Q_EMIT editingFinished();
    }
}

void AComboBox::focusInEvent(QFocusEvent *e)
{
    text_changed_ = false;
    QComboBox::focusInEvent(e);
}

void AComboBox::onTextChange(const QString&)
{
    text_changed_ = true;
}

void AComboBox::onActivate(const QString&)
{
    text_changed_ = false;
    Q_EMIT editingFinished();
}

// alComboBox
alComboBox::alComboBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
    alWidgetPre<AComboBox>(attr,WComboBox,id,parent)
{
    counter_ = 0;
    connect(wnd_, SIGNAL(editingFinished()), this, SLOT(onUpdate()));
}

void alComboBox::setAttr(const QString& name,const QString& value)
{
	if (QStringLiteral("append-row") == name)
	{
		QStringList data = value.split(QLatin1Char(';'));
		if (data[1].isEmpty())
			wnd_->addItem(data[0]);
		else
			wnd_->addItem(getPixmap(data[1]),data[0]);
		counter_ = wnd_->count();
	}
	else if (QStringLiteral("rows") == name)
	{
	    wnd_->clear();
	    QStringList lst = value.split(QLatin1Char(';'));
	    QStringListIterator i(lst);
	    while( i.hasNext() )
	    {
		QString text = i.next();
		if( !i.hasNext() )
		    break;
		QString icon = i.next();
		if( icon.isEmpty() )
		    wnd_->addItem(text);
		else
		    wnd_->addItem(getPixmap(icon), text);
	    }
	}
	else if (QStringLiteral("current") == name)
	{
		bool ok = false;
		int idx = value.toInt(&ok);
		if(!ok)
		    idx = -1;
		wnd_->setCurrentIndex(idx);
	}
	else if (QStringLiteral("current-text") == name)
	{
		wnd_->setEditText(value);
	}
	else if (QStringLiteral("alterability") == name)
		wnd_->setEditable(value == QStringLiteral("true"));
	else if (QStringLiteral("rows-clear") == name)
	{
		wnd_->clear();
		counter_ = 0;
	}
	else if (QStringLiteral("remove-row") == name)
	{
		bool ok = false;
		int ivalue = value.toInt(&ok);
		if(ok)
		    wnd_->removeItem(ivalue);
		counter_ = wnd_->count();
	}
	else if (QStringLiteral("row-item-text") == name)
	{
		QStringList data = value.split(QLatin1Char(';'));
		if( data.size() >= 2 )
		{
		    bool ok = false;
		    int ivalue = data[1].toInt(&ok);
		    if(ok)
			wnd_->setItemText(ivalue,data[0]);
		}
	}
	else if (QStringLiteral("row-item-pixmap") == name)
	{
		QStringList data = value.split(QLatin1Char(';'));
		if( data.size() >= 2 )
		{
		    bool ok = false;
		    int ivalue = data[1].toInt(&ok);
		    if(ok)
			wnd_->setItemIcon(ivalue, getPixmap(data[0]));
		}
	}
	else
		alWidget::setAttr(name,value);
}

void alComboBox::registerEvent(const QString& name)
{
    if (QStringLiteral("selected") == name)
    {
        connect(wnd_,SIGNAL( editingFinished() ),SLOT(onSelect()));
    }
    else if (QStringLiteral("changed") == name)
    {
        connect(wnd_,SIGNAL( editingFinished() ),SLOT(onChange()));
    }
}

QString alComboBox::postData() const
{
	QString post = QString(QStringLiteral(" (current . %1 )")).arg(wnd_->currentIndex());
	if (wnd_->isEditable() && (counter_ != wnd_->count()))
	{//reset items on alterator
	    post += QStringLiteral("( rows . (");
	    for (int i=0;i<wnd_->count();++i)
		post += QStringLiteral(" #(( \"") + Utils::simpleQuote(wnd_->itemText(i)) + QStringLiteral("\" . \"\"))");
	    post += QStringLiteral("))");
	}
	post += QString(QStringLiteral(" (current-text . \"%1\" )")).arg(Utils::simpleQuote(wnd_->currentText()));
	return post;
}
