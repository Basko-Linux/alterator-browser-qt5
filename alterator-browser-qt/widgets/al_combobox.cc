#include <QApplication>

#include "al_combobox.hh"
#include "a_pixmaps.hh"

AComboBox::AComboBox(QWidget *parent):
    QComboBox(parent)
{
    text_changed_ = false;
    connect(this, SIGNAL(editTextChanged(const QString&)),
	this, SLOT(onTextChange(const QString&)));
    connect(this, SIGNAL(activated(const QString&)),
	this, SLOT(onActivate(const QString&)));
}

AComboBox::~AComboBox() {}

void AComboBox::keyPressEvent(QKeyEvent *e)
{
    QComboBox::keyPressEvent(e);
    switch( e->key() )
    {
	case Qt::Key_Enter:
	case Qt::Key_Return:
	{
	    emit editingFinished();
	    break;
	}
	default:
	    break;
    }
}

void AComboBox::focusOutEvent(QFocusEvent* e)
{
    Qt::FocusReason reason = e->reason();
    if( reason != Qt::PopupFocusReason
        && !(QApplication::activePopupWidget() && QApplication::activePopupWidget()->parentWidget() == this))
    {
	if( text_changed_ )
	    emit editingFinished();
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
    emit editingFinished();
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
	if ("append-row" == name)
	{
		QStringList data = value.split(";");
		if (data[1].isEmpty())
			wnd_->addItem(data[0]);
		else
			wnd_->addItem(QIcon(getPixmap(data[1])),data[0]);
		counter_ = wnd_->count();
	}
	else if ("rows" == name)
	{
	    wnd_->clear();
	    QStringList lst = value.split(";");
	    QStringListIterator i(lst);
	    while( i.hasNext() )
	    {
		QString text = i.next();
		if( !i.hasNext() ) break;
		QString icon = i.next();
		if( icon.isEmpty() )
		    wnd_->addItem(text);
		else
		    wnd_->addItem(QIcon(getPixmap(icon)), text);
	    }
	}
	else if ("current" == name)
	{
		bool ok = false;
		int idx = value.toInt(&ok);
		if(!ok) idx = -1;
		wnd_->setCurrentIndex(idx);
	}
	else if ("current-text" == name)
	{
		wnd_->setEditText(value);
	}
	else if ("alterability" == name)
		wnd_->setEditable(value == "true");
	else if ("rows-clear" == name)
	{
		wnd_->clear();
		counter_ = 0;
	}
	else if ("remove-row" == name)
	{
		wnd_->removeItem(value.toInt());
		counter_ = wnd_->count();
	}
	else if ("row-item-text" == name)
	{
		QStringList data = value.split(";");
		wnd_->setItemText(data[1].toInt(),data[0]);
	}
	else if ("row-item-pixmap" == name)
	{
		QStringList data = value.split(";");
		wnd_->setItemIcon(data[1].toInt(),QIcon(getPixmap(data[0])));
	}
	else
		alWidget::setAttr(name,value);
}

void alComboBox::registerEvent(const QString& name)
{
    if ("selected" == name)
    {
        //connect(wnd_,SIGNAL( activated(int) ),SLOT(onSelect(int)));
        //connect(wnd_,SIGNAL( editTextChanged(const QString&) ),SLOT(onSelect(const QString&)));
        //connect(wnd_,SIGNAL( activated(const QString&) ),SLOT(onSelect(const QString&)));
        connect(wnd_,SIGNAL( editingFinished() ),SLOT(onSelect()));
    }
}

QString alComboBox::postData() const
{
	QString post = QString(" (current . %1 )").arg(wnd_->currentIndex());
	if (wnd_->isEditable() && (counter_ != wnd_->count()))
	{//reset items on alterator
	    post += "( rows . (";
	    for (int i=0;i<wnd_->count();++i)
		post += " #(( \""+ Utils::simpleQuote(wnd_->itemText(i))+ "\" . \"\"))";
	    post += "))";
	}
	post += QString(" (current-text . \"%1\" )").arg(Utils::simpleQuote(wnd_->currentText()));
	return post;
}
