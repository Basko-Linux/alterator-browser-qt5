
#include <QScrollArea>

#include "global.hh"
#include "widgets.hh"
#include "al_wizard_face.hh"
#include "a_pixmaps.hh"

extern alWizardFace *wizard_face;

// AWizardFace
AWizardFace::AWizardFace( QWidget *parent, Qt::WFlags f):
    QWidget(parent, f)
{
    labels_widget = new QFrame(this);
    labels_widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    labels_widget->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);

    title = new QLabel(this);
    title->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    title->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    title->setAlignment(Qt::AlignCenter);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scroll->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    scroll->setWidgetResizable( true );

    view_widget = new QWidget(scroll);
//    view_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scroll->setWidget(view_widget);

    buttons_widget = new QFrame(this);
    buttons_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    buttons_widget->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);

    labels_layout = new QVBoxLayout( labels_widget );
    buttons_layout = new QHBoxLayout( buttons_widget );
    main_layout = new QGridLayout(this);
    main_layout->addWidget( labels_widget, 0, 0, 2, 1 );
    main_layout->addWidget( title, 0, 1 );
    main_layout->addWidget( scroll, 1, 1);
    main_layout->addWidget( buttons_widget, 2, 0, 1, 2);

    signal_mapper = new QSignalMapper(this);
    connect(signal_mapper, SIGNAL(mapped(const QString &)),
	this, SIGNAL(itemSelected(const QString &)));
}

AWizardFace::~AWizardFace()
{}

int AWizardFace::findButtonPosition(ItemType type)
{
    int pos = -1;
    QMapIterator<QString, ItemType> it(types);
    while( it.hasNext() )
    {
	it.next();
	pos++;
	if( it.value() == type )
	{
	    QAbstractButton *btn = buttons[it.key()];
	    if( btn )
	    {
		pos = buttons_layout->indexOf( btn );
	    }
	    break;
	}
    }
    return pos;
}

int AWizardFace::newButtonPosition(ItemType type)
{
    switch( type )
    {
	case ItemHelp:
	    {
		return 0;
	    }
	case ItemApply:
	    {
		int pos = findButtonPosition( ItemCancel );
		if( pos >= 0 )
		    return pos;
		else
		    pos = findButtonPosition( ItemBackward );
		if( pos >= 0 )
		    return pos;
		else
		    return findButtonPosition( ItemForward );
	    }
	case ItemCancel:
	    {
		int pos = findButtonPosition( ItemApply );
		if( pos >= 0 )
		    return ++pos;
		else
		    pos = findButtonPosition( ItemBackward );
		if( pos >= 0 )
		    return pos;
		else
		    return findButtonPosition( ItemForward );
	    }
	case ItemBackward:
	    {
		int pos = findButtonPosition( ItemForward );
		if( pos >= 0 )
		    return pos;
		else
		    return -1;
	    }
	case ItemForward:
	    {
		return -1;
	    }
	case ItemGeneric:
	default:
	    {
		return 1;
	    }
    }
}

Qt::Alignment AWizardFace::newButtonAlignment(ItemType type)
{
    switch( type )
    {
	case ItemHelp:
	    {
		return Qt::AlignLeft;
	    }
	case ItemApply:
	case ItemCancel:
	case ItemForward:
	case ItemBackward:
	    {
		return Qt::AlignRight;
	    }
	case ItemGeneric:
	default:
	    {
		return Qt::AlignCenter;
	    }
    }
}

void AWizardFace::setButtonIcon(QAbstractButton* btn, ItemType type)
{
    QString name;
    switch( type )
    {
	case ItemHelp:
	    {
		name = "theme:help";
		break;
	    }
	case ItemApply:
	    {
		name = "theme:apply";
		break;
	    }
	case ItemCancel:
	    {
		name = "theme:cancel";
		break;
	    }
	case ItemForward:
	    {
		name = "theme:forward";
		break;
	    }
	case ItemBackward:
	    {
		name = "theme:backward";
		break;
	    }
	default:
	    break;
    }
    if( !name.isEmpty() && btn )
	btn->setIcon(QIcon(getPixmap(name)));
}

QWidget* AWizardFace::addItem(const QString &key, AWizardFace::ItemType type)
{
    QWidget *w = 0;
    
    switch( type )
    {
	case ItemGeneric:
	case ItemQuit:
	case ItemHelp:
	case ItemApply:
	case ItemCancel:
	case ItemBackward:
	case ItemForward:
	    {
		QPushButton *b = new QPushButton(buttons_widget);
		setButtonIcon(b, type);
		buttons_layout->insertWidget( newButtonPosition(type), b, 0, newButtonAlignment(type) );
		w = buttons[key] = b;
		types[key] = type;
		connect(b, SIGNAL(clicked()), signal_mapper, SLOT(map()));
		signal_mapper->setMapping(b, key);
		break;
	    }
	case ItemSection:
	case ItemStep:
	    {
		QPushButton *l = new QPushButton(labels_widget);
		l->setFlat(true);
		labels_layout->addWidget(l);
		w = buttons[key] = l;
		types[key] = type;
		connect(l, SIGNAL(clicked()), signal_mapper, SLOT(map()));
		signal_mapper->setMapping(l, key);
		break;
	    }
	default:
	    {
		break;
	    }
    }
    return w;
}

QWidget* AWizardFace::getView()
{
    return view_widget;
}

QWidget* AWizardFace::getItemWidget(const QString &key)
{
    if( buttons.contains(key) )
	return buttons[key];
    else
	return 0;
}

void AWizardFace::setItemText(const QString &key, const QString &value)
{
    //qDebug("%s: AWizardFace::setItemText id<%s> value<%s>", __FUNCTION__, id.toLatin1().data(), value.toLocal8Bit().data());
    if( buttons.contains(key) )
	buttons[key]->setText(value);
}

void AWizardFace::setItemPixmap(const QString &key, const QString &value)
{
    if( buttons.contains(key) )
	buttons[key]->setIcon(QIcon(getPixmap(value)));
}

void AWizardFace::setItemActivity(const QString &key, bool a)
{
    if( buttons.contains(key) )
	buttons[key]->setEnabled(a);
}

void AWizardFace::setTitle( const QString &value)
{
    title->setText(value);
}

void AWizardFace::setCurrentStep( int n )
{
    QList<QPushButton *> labels = labels_widget->findChildren<QPushButton *>();
    int i = 0;
    QList<QPushButton *>::iterator it;
    for( it = labels.begin();; it++, i++)
    {
	if( it == labels.end() )
	{
	    it--;
	    break;
	}
	if( i >= n )
	    break;
	QPushButton *lbl = *it;
	lbl->setEnabled(false);
	QFont font = lbl->font();
	font.setWeight(QFont::Normal);
	lbl->setFont(font);
    }
    QPushButton *current = *it;
    QFont font = current->font();
    current->setEnabled(true);
    font.setWeight(QFont::Bold);
    current->setFont(font);
    for(++it ;it != labels.end(); it++)
    {
	QPushButton *lbl = *it;
	lbl->setEnabled(true);
	QFont font = lbl->font();
	font.setWeight(QFont::Normal);
	lbl->setFont(font);
    }
}


void AWizardFace::cleanRequest()
{
    QList<QAbstractButton*> children;
    children += labels_widget->findChildren<QAbstractButton*>();
    children += buttons_widget->findChildren<QAbstractButton*>();
    QListIterator<QAbstractButton*> it(children);
    while( it.hasNext() )
    {
	QAbstractButton* b = it.next();
	b->deleteLater();
    }
}

QString AWizardFace::current()
{
    return current_;
}

void AWizardFace::onSelect(const QString& key)
{
    current_ = key;
    emit itemSelected();
}

// alWizardFace
alWizardFace::alWizardFace(const QString& id,const QString& parent):
    alWidgetPre<AWizardFace>(WizardFace,id,parent)
{
    new QVBoxLayout(wnd_->getView());
    key2type["quit"]     = AWizardFace::ItemQuit;
    key2type["help"]     = AWizardFace::ItemHelp;
    key2type["apply"]    = AWizardFace::ItemApply;
    key2type["cancel"]   = AWizardFace::ItemCancel;
    key2type["backward"] = AWizardFace::ItemBackward;
    key2type["forward"]  = AWizardFace::ItemForward;
    key2type["step"]     = AWizardFace::ItemStep;
    key2type["section"]  = AWizardFace::ItemSection;
}

alWizardFace::~alWizardFace(){}

QWidget* alWizardFace::getViewWidget()
{
    return wnd_->getView();
}

QLayout* alWizardFace::getViewLayout()
{
    return wnd_->getView()->layout();
}

void alWizardFace::registerEvent(const QString&)
{
}

QString alWizardFace::postData() const
{
    QString current = wnd_->current();
    if (current.isEmpty())
	return "";
    else
	return "(current-action . " +current+" )";
}

void alWizardFace::addItem(const QString& key, const QString& name, const QString& pixmap)
{
    if( !key.isEmpty() )
    {
        AWizardFace::ItemType type = key2type[key];
        if( key.isEmpty() )
	    type = AWizardFace::ItemGeneric;
	QWidget *w = wnd_->addItem(key, type);
	wnd_->setItemText(key, name);
	if (!pixmap.isEmpty())
	    wnd_->setItemPixmap(key, pixmap);
    }
}

void alWizardFace::setAttr(const QString& name,const QString& value)
{
    if( "title" == name )
    {
	// set and continue
	wnd_->setTitle(value);
    }

    if( "actions" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	const int len = data.size();
	for(int i=0;i+2 < len;i+=3)
	    addItem(data[i], data[i+1], data[i+2]);
    }
    if( "action-add" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	const int len = data.size();
	if( len >= 3 )
	    addItem(data[0], data[1], data[2]);
	else if( len >= 2 )
	    addItem(data[0], data[1], "");
	else if( len >= 1 )
	    addItem(data[0], "", "");
    }
    if( "action-activity" == name )
    {
	QStringList data = value.split(";");
	if( data.size() >= 2 )
	{
	    wnd_->setItemActivity(data[0], "true" == data[1]);
	}
    }
    else
	alWidget::setAttr(name,value);
}
