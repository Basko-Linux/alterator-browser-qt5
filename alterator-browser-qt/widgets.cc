#include "global.hh"

#include "widgets.hh"
#include "a_pixmaps.hh"


////////////////////////////////////////////

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
}
////////////////////////////////////////////

//all current elements on viewer
QMap<QString,alWidget*> elements;
QString help_source;

void updateHelp(const QString& value)
{
	help_source = value;
	QMapIterator<QString,alWidget*> i(elements);
	while (i.hasNext())
	{
		i.next();
		if (dynamic_cast<alHelpPlace*>(i.value()))
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

		QLayout *playout = findViewLayout(parent_);
		if( playout )
		{
		    if( MyBoxLayout *ml = qobject_cast<MyBoxLayout*>(playout) )
		    {
			int w = policies.at(0).toInt();
			int h = policies.at(1).toInt();
			int d = -1;
			if (policies.count() > 2)
			    d = convertAlign(policies.at(2));
			ml->addWidget(getWidget(),QSize(w,h),d);
		    }
		    else
			qDebug("don't set layout-policy");
		}
		else
		    qDebug("no layout");
	}
	else if ("help" == name)
		updateHelp(value);
}

////////////////////////////////////////////
void alLabel::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("pixmap" == name)
		wnd_->setPixmap( getPixmap(value) );
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
	if( "changed" == name )
		connect(wnd_,SIGNAL( textEdited(const QString&) ),SLOT(onChange(const QString&)));
	else if( "return-pressed" == name )
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
	else if ("text-append" == name)
		wnd_->append(value);
	else if ("append-text" == name)
		wnd_->append(value);
	else if ("alterability" == name)
		wnd_->setReadOnly("false" == value);
	else
		alWidget::setAttr(name,value);
}

void alTextBox::registerEvent(const QString& name)
{
	if ("changed" == name)
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
	else if ("state" == name)
		wnd_->setChecked("true" ==  value);
	else
		alWidget::setAttr(name,value);
}

void alGroupBox::registerEvent(const QString& name)
{
	if ("toggled" == name)
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
	if ("toggled" == name)
		connect(wnd_,SIGNAL( stateChanged(int) ),SLOT(onToggle(int)));
}

QString alCheckBox::postData() const
{
	return QString(" (state . ") + (wnd_->isChecked()?"#t":"#f")+" )";
}

void alListBox::setAttr(const QString& name,const QString& value)
{
	if ("append-row" == name)
	{//TODO:will be support for multiple columns here
		QStringList data = value.split(";");
		QListWidgetItem *item(new QListWidgetItem(wnd_));
		if( !data[0].isEmpty() )
			item->setText(data[0]);
		if( !data[1].isEmpty() )
			item->setIcon( QIcon(getPixmap(data[1])) );
		else
			item->setIcon( QIcon(getPixmap("theme:null")) );
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
		QListWidgetItem *item;
		if( icon.isEmpty() )
		    item = new QListWidgetItem(QIcon(getPixmap("theme:null")), text, wnd_);
		else
		    item = new QListWidgetItem(QIcon(getPixmap(icon)), text, wnd_);
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
	else if ("remove-row" == name)
	{
		if ("all" == value)
			wnd_->clear();
		else
			delete wnd_->takeItem(value.toInt());
	}
	else if ("row-item-text" == name)
	{
		QStringList data = value.split(";");
		wnd_->item(data[1].toInt())->setText(data[0]);
	}
	else if ("row-item-pixmap" == name)
	{
		QStringList data = value.split(";");
		wnd_->item( data[1].toInt() )->setIcon( QIcon(getPixmap(data[0])) );
	}
	else
	    alWidget::setAttr(name,value);

}

void alListBox::registerEvent(const QString& name)
{
	if ("selected" == name)
		connect( wnd_, SIGNAL(currentRowChanged(int)), SLOT(onSelect(int)) );
	else if ("clicked" == name)
	{
		connect( wnd_, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(onClick(QListWidgetItem*)) );
		connect(wnd_,SIGNAL(spaceBtnPressed()), SLOT(onClick()));
	}
	else if ("return-pressed" == name)
		connect( wnd_, SIGNAL(itemActivated(QListWidgetItem*)), SLOT(onReturn(QListWidgetItem*)) );
	else if ("double-clicked" == name)
		connect( wnd_, SIGNAL(itemDoubleClicked(QListWidgetItem*)), SLOT(onReturn(QListWidgetItem*)) );
}

QString alListBox::postData() const
{
	return QString(" (current . %1 )").arg(wnd_->currentRow());
}

void alMultiListBox::setAttr(const QString& name,const QString& value)
{
	if ("append-row" == name)
	{
		QStringList data = value.split(";");
		const int len = data.size();
		if (len <= 1) return;
		const int columns = wnd_->columnCount();
		QTreeWidgetItem *item(new QTreeWidgetItem(wnd_));
		for (int col = 0;(col < columns*2) && (col+1 < len);col+=2)
		{
			if (!data[col].isEmpty())
				item->setText(col/2,data[0]);
			if (!data[col+1].isEmpty())
				item->setIcon(col/2,QIcon(getPixmap(data[col+1])));
			else
				item->setIcon(col/2,QIcon(getPixmap("theme:null")));
		}
	}
	if ("rows" == name)
	{
		QStringList data = value.split(";");
		QList<QTreeWidgetItem *> items;
		const int len = data.size();
		const int columns = wnd_->columnCount();
		for(int i=0;i+1 < len;)
		{
			QTreeWidgetItem *elt = new QTreeWidgetItem(0);
			for(int col = 0; (col < columns);++col,i+=2)
			{
				if (!data[i].isEmpty())
					elt->setText(col,data[i]);
				if (!data[i+1].isEmpty())
					elt->setIcon(col,QIcon(getPixmap(data[i+1])));
				else
					elt->setIcon(col,QIcon(getPixmap("theme:null")));
			}
			//items.push_front(elt);
			items.push_back(elt);
		}
		wnd_->clear();		
		wnd_->addTopLevelItems(items);
	}
	else if ("current" == name)
	{
		QTreeWidgetItem *i = wnd_->topLevelItem(value.toInt());
		wnd_->scrollToItem(i);
		wnd_->setCurrentItem(i);
	}
	else if ("remove-row" == name)
	{
		if ("all" == value)
			wnd_->clear();
		else
			delete wnd_->takeTopLevelItem(value.toInt());
	}
	else if ("row-item-text" == name)
	{
		QStringList data = value.split(";");
		int column = data.size()<3? 0: data[2].toInt();
		wnd_->topLevelItem(data[1].toInt())->setText(column,data[0]);
	}
	else if ("row-item-pixmap" == name)
	{
		QStringList data = value.split(";");
		int column = data.size()<3? 0: data[2].toInt();
		wnd_->topLevelItem(data[1].toInt())->setIcon(column,QIcon(getPixmap(data[0])));
	}
	else if ("header" == name)
	{
		wnd_->header()->show();
		wnd_->setHeaderLabels(value.split(";"));
	}
	else
		alWidget::setAttr(name,value);
}

void alMultiListBox::registerEvent(const QString& name)
{
	if ("selected" == name)
		connect(wnd_,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
			     SLOT(onSelect(QTreeWidgetItem*,QTreeWidgetItem*)));
	else if ("clicked" == name)
	{
		connect(wnd_,SIGNAL(itemPressed(QTreeWidgetItem*,int)), SLOT(onClick(QTreeWidgetItem*,int)));
		connect(wnd_,SIGNAL(spaceBtnPressed()), SLOT(onClick()));
	}
	else if ("return-pressed" == name)
		connect( wnd_, SIGNAL(itemActivated(QTreeWidgetItem*,int)), SLOT(onReturn(QTreeWidgetItem*,int)));
	else if ("double-clicked" == name)
		connect(wnd_,SIGNAL(itemDoubleClicked(QTreeWidgetItem*,int)), SLOT(onDoubleClick(QTreeWidgetItem*,int)));
}

QString alMultiListBox::postData() const
{
	QTreeWidgetItem *i = wnd_->currentItem();
	if( i )
	    return QString(" (current . ") + QString::number(wnd_->indexOfTopLevelItem(i)) +" )";
	else
	    return "";
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
	else if ("remove-row" == name)
	{
		if ("all" == value)
			wnd_->clear();
		else
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
		post += " #(( \""+ simpleQuote(wnd_->itemText(i))+ "\" . \"\"))";
	    post += "))";
	}
	return post;
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
		if (elements.contains(value))
			wnd_->setCurrentWidget(elements[value]->getWidget());
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
			wnd_->showFullScreen();
		else
			wnd_->showNormal();
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
			wnd_->showFullScreen();
		else
			wnd_->showNormal();
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

void alTree::setAttr(const QString& name,const QString& value)
{
	if ("tree-items" == name)
	{
                wnd_->clear();//clear all previous content
		items_ = value;
		if (!coords_.isEmpty())
		{
		    setItems();
		    coordmap_ = coords_.split(";");//move to internal storage
		    items_ = coords_ = "";
		}
	}
	if ("coordinates" == name)
	{
		coords_ = value;
		if (!items_.isEmpty())
		{
		    setItems();
		    coordmap_ = coords_.split(";");//move to internal storage
		    items_ = coords_ = "";
		}
	}
	if ("current" == name)
	{
		QStringList coords = value.split(";");
		QTreeWidgetItem *item = findPosition(wnd_->topLevelItem(coords[0].toInt()),
			                                                coords.mid(1),0);
		wnd_->scrollToItem(item);
		wnd_->setCurrentItem(item);
	}
	else if ("header" == name) 
	{ 
		wnd_->header()->show(); 
        	wnd_->setHeaderLabels(value.split(";")); 
	} 
	else
		alWidget::setAttr(name,value);
}

void alTree::registerEvent(const QString& name)
{
	if ("selected" == name)
		connect(wnd_,SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
			     SLOT(onSelect(QTreeWidgetItem*,QTreeWidgetItem*)));
}

QString alTree::postData() const
{
    QTreeWidgetItem *i = wnd_->currentItem();
    if (i)
    {
	const int columns = wnd_->columnCount();
	QString data = coordmap_[i->type()/(2*columns)];
        data.replace(","," ");
	return QString("(current .")+"("+data+"))";
    }
    return "";
}

QTreeWidgetItem *alTree::findPosition(QTreeWidgetItem *parent,QStringList coords,int deep)
{
	if (!parent)
		return 0;
	else if (coords.size() == deep)
		return parent;
	else
		return findPosition(parent->child(coords[0].toInt()),coords.mid(1),deep);
}

void alTree::setItems()
{
	QStringList itemlist = items_.split(";");
	QStringList coordlist = coords_.split(";");
	const int len = itemlist.size();
	const int columns = wnd_->columnCount();
	for (int i=0;i+1<len;)
	{
		QStringList coords = coordlist[i/(2*columns)].split(",");
		const int len = coords.size();
		
		QTreeWidgetItem *item;
		if (len == 1)
			item = new QTreeWidgetItem(wnd_,i);
		else
			item = new QTreeWidgetItem(findPosition(wnd_->topLevelItem(coords[0].toInt()),
			                                                         coords.mid(1),1),
								    i);
		
		for(int col=0;col < columns; ++col,i+=2)
		{
			QString data = itemlist[i];
			QString pixmap = itemlist[i+1];
			item->setText(col,data);
			if (!pixmap.isEmpty())
                		item->setIcon(col,QIcon(getPixmap(pixmap)));
		}
	}
}

void alHelpPlace::setAttr(const QString& name,const QString& value)
{
	if ("source" == name)
		wnd_->setSource(value);
	else
		alWidget::setAttr(name,value);
}

void alHelpPlace::onAnchor(const QUrl& url)
{
	wnd_->setSource(url);
}

void alSlider::setAttr(const QString& name,const QString& value)
{
	if( "orientation" == name )
	{
	    if( "vertical" == value )
		wnd_->setOrientation(Qt::Vertical);
	    else
		wnd_->setOrientation(Qt::Horizontal);
	}
	else if( "minimum" == name )
	    wnd_->setMinimum( value.toInt() );
	else if( "maximum" == name )
	    wnd_->setMaximum( value.toInt() );
	else if( "step" == name )
	{
	    int step = value.toInt();
	    wnd_->setSingleStep( step );
	    wnd_->setPageStep( step * 8 );
	}
	else if( "value" == name )
	    wnd_->setValue(value.toInt());
	else
	    alWidget::setAttr(name,value);
}

void alSlider::registerEvent(const QString& name)
{
	if ("changed" == name)
		connect(wnd_, SIGNAL(sliderMoved(int)), SLOT(onChange(int)));
}

QString alSlider::postData() const
{
	return QString(" (value . %1 )").arg(wnd_->value());
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
