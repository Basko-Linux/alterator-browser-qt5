#include "global.hh"

#include "widgets.hh"
#include "a_pixmaps.hh"
#include "utils.hh"


void updateHelp(const QString& value)
{
	QMapIterator<QString,alWidget*> i(elements);
	while (i.hasNext())
	{
		i.next();
		if( (i.value())->type() == alWidget::HelpPlace )
		{
			i.value()->setAttr("source",value);
			break;
		}
	}	
}

QLayout *findViewLayout(const QString& id)
{
	if (!elements.contains(id)) return 0;
	alWidget *aw = elements[id];
	return aw->getViewLayout();
}

QWidget* findQWidget(const QString& id)
{
	if (!elements.contains(id)) return 0;
	QWidget *w = elements[id]->getWidget();
	if( w )	return w;
	return 0;
}

alWidget* findAlWidget(const QString& id)
{
	if (!elements.contains(id)) return 0;
	return elements[id];
}

alWidget *findAlWidgetByName(const QString& name)
{
    QMapIterator<QString,alWidget*> it(elements);
    while(it.hasNext())
    {
	it.next();
	alWidget *item = it.value();
	if (item->objectName() == name)
	{
	    return item;
	}
    }
    return 0;
}

QList<alWidget*> findAlChildren(const QString& id)
{
    QList<alWidget*> children;
    QMapIterator<QString,alWidget*> it(elements);
    while(it.hasNext())
    {
	it.next();
	alWidget *aw = it.value();
	if( aw->getParentId() == id )
	    children << aw;
    }
    return children;
}

////////////////////////////////////////////
void alLabel::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("pixmap" == name)
		wnd_->setPixmap( getPixmap(value) );
//	else if ("align" == name)
//	{
//		wnd_->setAlignment(Utils::convertAlign(value));
//	}
	else
		alWidget::setAttr(name,value);
}

void alButton::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("pixmap" == name)
		wnd_->setIcon(QIcon(getPixmap(value)));
	else
		alWidget::setAttr(name,value);
}

void alButton::registerEvent(const QString& name)
{
	if ("clicked" == name)
		connect(wnd_,SIGNAL( clicked(bool) ),SLOT(onClick(bool)));
}



void alRadio::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("state" == name)
		wnd_->setChecked("true" ==  value);
	else
		alWidget::setAttr(name,value);
}

void alRadio::registerEvent(const QString& name)
{
	if ("toggled" == name)
		connect(wnd_,SIGNAL( toggled(bool) ),SLOT(onToggle(bool)));
}

QString alRadio::postData() const
{
	return QString(" (state . ") + (wnd_->isChecked()?"#t":"#f")+" )";
}

void alCheckBox::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("state" == name)
		wnd_->setChecked("true" ==  value);
	else
		alWidget::setAttr(name,value);
}

void alCheckBox::registerEvent(const QString& name)
{
	if ("toggled" == name)
		connect(wnd_,SIGNAL( stateChanged(int) ),SLOT(onToggle(int)));
}

QString alCheckBox::postData() const
{
	return QString(" (state . ") + (wnd_->isChecked()?"#t":"#f")+" )";
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

void alProgressBar::setAttr(const QString& name,const QString& value)
{
    if( "value" == name )
	wnd_->setValue(value.toInt());
    else if( "maximum" == name )
	wnd_->setMaximum(value.toInt());
    else if( "minimum" == name )
	wnd_->setMinimum(value.toInt());
    else
	alWidget::setAttr(name,value);
}

void alHelpPlace::setAttr(const QString& name,const QString& value)
{
    if("source" == name)
	main_window->setHelpSource(value);
}

void alSplitter::setAttr(const QString& name,const QString& value)
{
	if( "orientation" == name )
	{
	    if( "vertical" == value )
		wnd_->setOrientation(Qt::Vertical);
	    else
		wnd_->setOrientation(Qt::Horizontal);
	}
	else
	    alWidget::setAttr(name,value);
}
