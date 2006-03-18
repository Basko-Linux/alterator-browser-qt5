#include "widgets.hh"


////////////////////////////////////////////

QString imagePath = "/usr/share/alterator/images/";

namespace
{
	Qt::Alignment convertAlign(const QString& value)
	{
		if ("left" == value) return  Qt::AlignLeft;
		else if ("right" == value) return Qt::AlignRight;
		else if ("center" == value) return Qt::AlignCenter;
		return Qt::AlignCenter;
	}

	QLineEdit::EchoMode convertEchoMode(const QString& value)
	{
		if ("no" == value) return QLineEdit::NoEcho;
		else if ("stars" == value) return QLineEdit::Password;
		return QLineEdit::Normal; //default yes
	}

	alWidget *findWidget(const QString& name)
	{
		QMapIterator<QString,alWidget*> it(elements);
		while(it.hasNext())
		{
			it.next();
			if ( it.value()->getWidget()->objectName() == name )
				return it.value();
		}
		return 0;
	}
	
}
////////////////////////////////////////////

//all current elements on viewer
QMap<QString,alWidget*> elements;

QLayout *findLayout(const QString& id)
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

void alWidget::setAttr(const QString& name,const QString& value)
{
	if ("visibility" == name)
		("true" == value)?getWidget()->show():getWidget()->hide();
	else if ("activity" == name)
		getWidget()->setEnabled("true" == value);
	else if ("widget-name" == name)
		getWidget()->setObjectName(value);
	else if ("tooltip" == name)
	{
		QStringList args = value.split(";");
		getWidget()->setToolTip(args[0]);
	}
	else if ("layout-policy" == name)
	{
		QStringList policies = value.split(";");
		if (policies.count() < 2)
		{
		    qDebug("wrong number of arguments to layout-policy attribute");
		    exit(1);
		}
		int w = policies.at(0).toInt();
		int h = policies.at(1).toInt();
		int d = -1;
		if (policies.count() > 2) d = convertAlign(policies.at(2));

		QLayout *playout = findLayout(parent_);
		if( playout )
		{
		    if( MyBoxLayout *ml = qobject_cast<MyBoxLayout*>(playout) )
			ml->addWidget(getWidget(),QSize(w,h),d);
		    else
			qDebug("don't set layout-policy");
		}
	}
}

////////////////////////////////////////////
void alLabel::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("pixmap" == name)
		wnd_->setPixmap(QPixmap(imagePath+value));
	else if ("align" == name)
		wnd_->setAlignment(convertAlign(value));
	else
		alWidget::setAttr(name,value);
}

void alButton::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("pixmap" == name)
		wnd_->setIcon(QIcon(imagePath+value));
	else
		alWidget::setAttr(name,value);
}

void alButton::registerEvent(const QString& name)
{
	if ("on-click" == name)
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
	if ("on-toggle" == name)
		connect(wnd_,SIGNAL( toggled(bool) ),SLOT(onToggle(bool)));
}

QString alRadio::postData() const
{
	return QString(" (state . ") + (wnd_->isChecked()?"#t":"#f")+" )";
}


void alEdit::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("echo" == name) 
		wnd_->setEchoMode(convertEchoMode(value));
	else if ("alterability" == name)
		wnd_->setReadOnly("false" == value);
	else
		alWidget::setAttr(name,value);
}

void alEdit::registerEvent(const QString& name)
{
	if( "on-change" == name )
		connect(wnd_,SIGNAL( textEdited(const QString&) ),SLOT(onChange(const QString&)));
	else if( "on-return" == name )
	    connect(wnd_,SIGNAL(returnPressed() ),SLOT(onReturn()));
}


QString alEdit::postData() const
{
	return " (text . \""+simpleQuote(wnd_->text())+"\" )";
}


void alTextBox::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setPlainText(value);
	if ("text-append" == name)
		wnd_->append(value);
	else if ("alterability" == name)
		wnd_->setReadOnly("false" == value);
	else
		alWidget::setAttr(name,value);
}

void alTextBox::registerEvent(const QString& name)
{
	if ("on-change" == name)
		connect(wnd_,SIGNAL( textChanged(const QString&) ),SLOT(onChange(const QString&)));
}

QString alTextBox::postData() const
{
	return " (text . \""+simpleQuote(wnd_->toPlainText())+"\" )";
}

void alGroupBox::setAttr(const QString& name,const QString& value)
{
	if ("title" == name)
		wnd_->setTitle(value);
	if ("alterability" == name)
	{
		wnd_->setCheckable("true" == value);
	}
	else if ("state" == name)
		wnd_->setChecked("true" ==  value);
	else
		alWidget::setAttr(name,value);
}

void alGroupBox::registerEvent(const QString& name)
{
	if ("on-toggle" == name)
		connect(wnd_,SIGNAL( toggled(bool) ),SLOT(onToggle(bool)));
}


QString alGroupBox::postData() const
{
	if (wnd_->isCheckable())
		return QString(" (state . ")+ (wnd_->isChecked()?"#t":"#f") +" )";
	else
		return "";
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
	if ("on-toggle" == name)
		connect(wnd_,SIGNAL( stateChanged(int) ),SLOT(onToggle(int)));
}

QString alCheckBox::postData() const
{
	return QString(" (state . ") + (wnd_->isChecked()?"#t":"#f")+" )";
}

void alListBox::setAttr(const QString& name,const QString& value)
{
	if ("append-item" == name)
	{//TODO:will be support for multiple columns here
		QStringList data = value.split(";");
		QListWidgetItem *item(new QListWidgetItem(wnd_));
		if (!data[0].isEmpty())
			item->setText(data[0]);
		if (!data[1].isEmpty())
			item->setIcon(QIcon(imagePath+data[1]));
	}
	else if ("items" == name)
	{
	    wnd_->clear();
	    QStringList lst = value.split(";");
	    QStringListIterator i(lst);
	    while( i.hasNext() )
	    {
		QString text = i.next();
		if( !i.hasNext() ) break;
		QString icon = i.next();
		QListWidgetItem *item;
		if( icon.isEmpty() )
		    item = new QListWidgetItem(text, wnd_);
		else
		    item = new QListWidgetItem(QIcon(imagePath+icon), text, wnd_);
	    }
	}
	else if ("current" == name)
	{
		int n = value.toInt();
		if( n >= 0)
		{
		    QListWidgetItem *i = wnd_->item(n);
		    wnd_->setCurrentRow(n);
		    wnd_->scrollToItem(i);
		}
	}
	else if ("remove-item" == name)
	{
		if ("all" == value)
			wnd_->clear();
		else
			delete wnd_->takeItem(value.toInt());
	}
	else if ("item-text" == name)
	{//TODO: will be support for multiple columns here
		QStringList data = value.split(";");
		wnd_->item(data[1].toInt())->setText(data[0]);
	}
	else if ("item-pixmap" == name)
	{//TODO: will be support for multiple columns here
		QStringList data = value.split(";");
		wnd_->item(data[1].toInt())->setIcon(QIcon(imagePath+data[0]));
	}
	else
		alWidget::setAttr(name,value);

}

void alListBox::registerEvent(const QString& name)
{
	if ("on-select" == name)
		connect( wnd_, SIGNAL(currentRowChanged(int)), SLOT(onSelect(int)) );
	else if ("on-click" == name)
		connect( wnd_, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(onClick(QListWidgetItem*)) );
	else if ("on-return" == name)
		connect( wnd_, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(onReturn(QListWidgetItem*)) );
}

QString alListBox::postData() const
{
	return QString(" (current . ") + QString("%1").arg(wnd_->currentRow()) +" )";
}

void alComboBox::setAttr(const QString& name,const QString& value)
{
	if ("append-item" == name)
	{
		QStringList data = value.split(";");
		if (data[1].isEmpty())
			wnd_->addItem(data[0]);
		else
			wnd_->addItem(QIcon(imagePath+data[1]),data[0]);
	}
	else if ("items" == name)
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
		    wnd_->addItem(QIcon(imagePath+icon), text);
	    }
	}
	else if ("current" == name)
		wnd_->setCurrentIndex(value.toInt());
	else if ("remove-item" == name)
	{
		if ("all" == value)
			wnd_->clear();
		else
			wnd_->removeItem(value.toInt());
	}
	else if ("item-text" == name)
	{
		QStringList data = value.split(";");
		wnd_->setItemText(data[1].toInt(),data[0]);
	}
	else if ("item-pixmap" == name)
	{
		QStringList data = value.split(";");
		wnd_->setItemIcon(data[1].toInt(),QIcon(imagePath+data[0]));
	}
	else
		alWidget::setAttr(name,value);
}

void alComboBox::registerEvent(const QString& name)
{
	if ("on-select" == name)
		connect(wnd_,SIGNAL( activated(int) ),SLOT(onSelect(int)));
}

QString alComboBox::postData() const
{
	return QString(" (current . ") + QString::number(wnd_->currentIndex()) +" )";
}



void alTabPage::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		static_cast<QTabWidget*>(tabbox_->getWidget())->setTabText(idx_,value);
	else 
	{
		if (("widget-name" == name) &&
		    (tabbox_ && tabbox_->current_ == value))
		   static_cast<QTabWidget*>(tabbox_->getWidget())->setCurrentIndex(idx_);

		alWidget::setAttr(name,value);
	}
}


void alTabBox::setAttr(const QString& name,const QString& value)
{
	if ("current" == name)
	{ //find current in elements or save value
		alWidget *w = findWidget(value);
		if (w)
			wnd_->setCurrentWidget(w->getWidget());
		else
			current_ = value;
	}
	else
		alWidget::setAttr(name,value);
}


void alDialog::setAttr(const QString& name,const QString& value)
{
	if ("caption" == name)
		wnd_->setWindowTitle(value);
	else if ("full-screen" == name)
	{
		if ("true" == value)
			wnd_->setWindowState(wnd_->windowState() | Qt::WindowFullScreen);
		else
			wnd_->setWindowState(wnd_->windowState() & (~Qt::WindowFullScreen));
	}
	else if ("width" == name)
		wnd_->resize(value.toInt(),wnd_->height());
	else if ("height" == name)
		wnd_->resize(wnd_->width(),value.toInt());
	else
		alWidget::setAttr(name,value);
}

void alMainWidget::setAttr(const QString& name,const QString& value)
{
	if ("caption" == name)
		wnd_->setWindowTitle(value);
	else if ("full-screen" == name)
	{
		if ("true" == value)
			wnd_->setWindowState(wnd_->windowState() | Qt::WindowFullScreen);
		else
			wnd_->setWindowState(wnd_->windowState() & (~Qt::WindowFullScreen));
	}
	else if ("layout-policy" == name)
	{
		//qDebug("don't do layout policy for main widget");
	}
	else if ("width" == name)
	{
		//wnd_->resize(value.toInt(),wnd_->height());
	}
	else if ("height" == name)
	{
		//wnd_->resize(wnd_->width(),value.toInt());
	}
	else
		alWidget::setAttr(name,value);
}

void alBox::setAttr(const QString& name,const QString& value)
{
	if ("margin" == name)
	{
	    QLayout *l = getViewLayout();
	    if( l )
		l->setMargin(value.toInt());
	}
	else if ("spacing" == name)
	{
	    QLayout *l = getViewLayout();
	    if( l )
		l->setSpacing(value.toInt());
	}
	else if("clear-layout" == name)
	{
	    QLayout *l = getViewLayout();
	    if( l )
		if( MyBoxLayout *ml = qobject_cast<MyBoxLayout*>(l) )
		    ml->deleteAllItems();
	}
	else if ("background-color" == name)
		wnd_->setBrush(QBrush(QColor(value)));
	else if ("children-align" == name)
	{
	    QLayout *l = getViewLayout();
	    if( l )
		if( MyBoxLayout *ml = qobject_cast<MyBoxLayout*>(l) )
		    ml->setChildrenAlign(convertAlign(value));
	}
	else
	    alWidget::setAttr(name,value);
}
