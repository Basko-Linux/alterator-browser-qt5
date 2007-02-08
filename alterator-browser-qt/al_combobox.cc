
#include "al_combobox.hh"
#include "a_pixmaps.hh"

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
		wnd_->setCurrentIndex(value.toInt());
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
		connect(wnd_,SIGNAL( activated(int) ),SLOT(onSelect(int)));
}

QString alComboBox::postData() const
{
	QString post = QString(" (current . ") + QString::number(wnd_->currentIndex()) +" )";
	if (wnd_->isEditable() && (counter_ != wnd_->count()))
	{//reset items on alterator
	    post += "( rows . (";
	    for (int i=0;i<wnd_->count();++i)
		post += " #(( \""+ Utils::simpleQuote(wnd_->itemText(i))+ "\" . \"\"))";
	    post += "))";
	}
	return post;
}

