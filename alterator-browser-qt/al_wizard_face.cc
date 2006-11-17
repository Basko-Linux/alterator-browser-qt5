
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
    key2type["abort"]    = AWizardFace::ActionAbort;
    key2type["finish"]   = AWizardFace::ActionFinish;
    key2type["help"]     = AWizardFace::ActionHelp;
    key2type["apply"]    = AWizardFace::ActionApply;
    key2type["cancel"]   = AWizardFace::ActionCancel;
    key2type["backward"] = AWizardFace::ActionBackward;
    key2type["forward"]  = AWizardFace::ActionForward;

    steps_widget = new QFrame(this);
    steps_widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Expanding);
    steps_widget->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);

    stepbox = new QListWidget(steps_widget);

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

    menu_btn = new QPushButton(buttons_widget);
    menu = new QMenu();
    menu_btn->setMenu(menu);

    steps_layout = new QVBoxLayout( steps_widget );
    buttons_layout = new QHBoxLayout( buttons_widget );
    main_layout = new QGridLayout(this);
    main_layout->addWidget( steps_widget, 0, 0, 2, 1 );
    main_layout->addWidget( title, 0, 1 );
    main_layout->addWidget( scroll, 1, 1);
    main_layout->addWidget( buttons_widget, 2, 0, 1, 2);
    steps_layout->addWidget( stepbox );
    buttons_layout->insertWidget(0, menu_btn, 0, Qt::AlignLeft);

    signal_mapper = new QSignalMapper(this);
    connect(signal_mapper, SIGNAL(mapped(const QString &)),
	this, SIGNAL(actionSelected(const QString &)));
    connect( this, SIGNAL(actionSelected(const QString&)),
	this, SLOT(onSelectAction(const QString&)) );
    connect( stepbox, SIGNAL(itemActivated(QListWidgetItem*)),
	this, SLOT(onSelectStep(QListWidgetItem*)) );
}

AWizardFace::~AWizardFace()
{}

int AWizardFace::findButtonPosition(ActionType type)
{
    int pos = -1;
    QMapIterator<QString, ActionType> it(button_types);
    while( it.hasNext() )
    {
	it.next();
	pos++;
	if( it.value() == type )
	{
	    if( buttons.contains(it.key()) )
	    {
		pos = buttons_layout->indexOf( buttons[it.key()] );
	    }
	    break;
	}
    }
    return pos;
}

int AWizardFace::newButtonPosition(ActionType type)
{
    switch( type )
    {
	case ActionHelp:
	    {
		return 0;
	    }
	case ActionApply:
	    {
		int pos = findButtonPosition( ActionCancel );
		if( pos >= 0 )
		    return pos;
		else
		    pos = findButtonPosition( ActionBackward );
		if( pos >= 0 )
		    return pos;
		else
		    return findButtonPosition( ActionForward );
	    }
	case ActionCancel:
	    {
		int pos = findButtonPosition( ActionApply );
		if( pos >= 0 )
		    return ++pos;
		else
		    pos = findButtonPosition( ActionBackward );
		if( pos >= 0 )
		    return pos;
		else
		    return findButtonPosition( ActionForward );
	    }
	case ActionBackward:
	    {
		int pos = findButtonPosition( ActionForward );
		if( pos >= 0 )
		    return pos;
		else
		    return -1;
	    }
	case ActionForward:
	    {
		return -1;
	    }
	case ActionGeneric:
	default:
	    {
		return 1;
	    }
    }
}

Qt::Alignment AWizardFace::newButtonAlignment(ActionType type)
{
    switch( type )
    {
	case ActionHelp:
	    {
		return Qt::AlignLeft;
	    }
	case ActionApply:
	case ActionCancel:
	case ActionForward:
	case ActionBackward:
	    {
		return Qt::AlignRight;
	    }
	case ActionGeneric:
	default:
	    {
		return Qt::AlignCenter;
	    }
    }
}

QPixmap AWizardFace::defaultActionIcon(ActionType type)
{
    QString name;
    switch( type )
    {
	case ActionFinish:
	    {
		name = "theme:finish"; // names are fake until fixed
		break;
	    }
	case ActionAbort:
	    {
		name = "theme:abort";
		break;
	    }
	case ActionHelp:
	    {
		name = "theme:help";
		break;
	    }
	case ActionApply:
	    {
		name = "theme:apply";
		break;
	    }
	case ActionCancel:
	    {
		name = "theme:cancel";
		break;
	    }
	case ActionForward:
	    {
		name = "theme:forward";
		break;
	    }
	case ActionBackward:
	    {
		name = "theme:backward";
		break;
	    }
	default:
	    break;
    }
    return getPixmap(name);
}

void AWizardFace::addAction(const QString& key, const QString& name, const QString& pixmap)
{
    if( !key.isEmpty() )
    {
        AWizardFace::ActionType type = key2type[key];
        if( key.isEmpty() )
	    type = AWizardFace::ActionGeneric;
	addAction(key, type);
	setActionText(key, name);
	if (!pixmap.isEmpty())
	    setActionPixmap(key, pixmap);
    }
}


void AWizardFace::addAction(const QString &key, AWizardFace::ActionType type)
{
    switch( type )
    {
	case ActionFinish:
	case ActionApply:
	case ActionCancel:
	case ActionBackward:
	case ActionForward:
	    {
		QPushButton *b = new QPushButton(buttons_widget);
		b->setIcon(QIcon(defaultActionIcon(type)));
		buttons_layout->insertWidget( newButtonPosition(type), b, 0, newButtonAlignment(type) );
		buttons[key] = b;
		button_types[key] = type;
		connect(b, SIGNAL(clicked()), signal_mapper, SLOT(map()));
		signal_mapper->setMapping(b, key);
		break;
	    }
	case ActionHelp:
	case ActionAbort:
	case ActionGeneric:
	default:
	    {
		QAction *a = menu->addAction("");
		a->setIcon(QIcon(defaultActionIcon(type)));
		menus[key] = a;
		connect(a, SIGNAL(changed()), signal_mapper, SLOT(map()));
		signal_mapper->setMapping(a, key);
		break;
	    }
    }
}

void AWizardFace::removeAction(const QString &key)
{
    if( buttons.contains(key) )
    {
	QAbstractButton *b = buttons.take(key);
	button_types.remove(key);
	b->deleteLater();
    }
    else if( menus.contains(key) )
    {
	menu->removeAction(menus[key]);
    }
}

void AWizardFace::clearActions()
{
    QMapIterator<QString, QAbstractButton*> it(buttons);
    while( it.hasNext() )
    {
	it.next();
	it.value()->deleteLater();
    }
    buttons.clear();
    button_types.clear();
    menu->clear();
    menus.clear();
}

void AWizardFace::addStep(const QString& name, const QString& pixmap)
{
    new QListWidgetItem(QIcon(getPixmap(pixmap)), name, stepbox);
}

void AWizardFace::removeStep(int n)
{
    QListWidgetItem *item = stepbox->takeItem(n);
    if( item )
    {
	delete item;
    }
}

void AWizardFace::clearSteps()
{
    stepbox->clear();    
}

QWidget* AWizardFace::getViewWidget()
{
    return view_widget;
}

void AWizardFace::setActionText(const QString &key, const QString &value)
{
    //qDebug("%s: AWizardFace::setItemText id<%s> value<%s>", __FUNCTION__, id.toLatin1().data(), value.toLocal8Bit().data());
    if( buttons.contains(key) )
	buttons[key]->setText(value);
    // FIXME menus
}

void AWizardFace::setActionPixmap(const QString &key, const QString &value)
{
    if( buttons.contains(key) )
	buttons[key]->setIcon(QIcon(getPixmap(value)));
    // FIXME menus
}

void AWizardFace::setActionActivity(const QString &key, bool a)
{
    if( buttons.contains(key) )
	buttons[key]->setEnabled(a);
    // FIXME menus
}

void AWizardFace::setStepText(int n, const QString &value)
{
    QListWidgetItem *item = stepbox->item(n);
    if( item )
	item->setText(value);
}

void AWizardFace::setStepPixmap(int n, const QString &value)
{
    QListWidgetItem *item = stepbox->item(n);
    if( item )
	item->setIcon(QIcon(getPixmap(value)));
}

void AWizardFace::setStepActivity(int n, bool a)
{
    QListWidgetItem *item = stepbox->item(n);
    if( item )
    {
	if( a )
	    item->setFlags(Qt::ItemIsEnabled);
	else
	    item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    }
}

void AWizardFace::setTitle( const QString &value)
{
    title->setText(value);
}

void AWizardFace::setCurrentStep( int n )
{
    stepbox->setCurrentRow(n);
/*
    QList<QPushButton *> steps = steps_widget->findChildren<QPushButton *>();
    int i = 0;
    QList<QPushButton *>::iterator it;
    for( it = steps.begin();; it++, i++)
    {
	if( it == steps.end() )
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
    for(++it ;it != steps.end(); it++)
    {
	QPushButton *lbl = *it;
	lbl->setEnabled(true);
	QFont font = lbl->font();
	font.setWeight(QFont::Normal);
	lbl->setFont(font);
    }
*/
}


void AWizardFace::cleanRequest()
{
/*
    QList<QAbstractButton*> children;
    children += steps_widget->findChildren<QAbstractButton*>();
    children += buttons_widget->findChildren<QAbstractButton*>();
    QListIterator<QAbstractButton*> it(children);
    while( it.hasNext() )
    {
	QAbstractButton* b = it.next();
	b->deleteLater();
    }
*/
}

QString AWizardFace::currentAction()
{
    return current_action;
}

int AWizardFace::currentStep()
{
    return stepbox->currentRow();
}

void AWizardFace::onSelectAction(const QString& key)
{
    current_action = key;
    emit actionSelected();
}

void AWizardFace::onSelectStep(QListWidgetItem*)
{
    emit stepSelected();
}


// alWizardFace
alWizardFace::alWizardFace(const QString& id,const QString& parent):
    alWidgetPre<AWizardFace>(WizardFace,id,parent)
{
    new QVBoxLayout(wnd_->getViewWidget());
}

alWizardFace::~alWizardFace(){}

QWidget* alWizardFace::getViewWidget()
{
    return wnd_->getViewWidget();
}

QLayout* alWizardFace::getViewLayout()
{
    return wnd_->getViewWidget()->layout();
}

void alWizardFace::registerEvent(const QString& name)
{
    if ("clicked" == name)
	connect(wnd_,SIGNAL(actionSelected()), SLOT(onClick()));
    if ("selected" == name)
	connect(wnd_,SIGNAL(stepSelected()), SLOT(onSelect()));
}

QString alWizardFace::postData() const
{
    QString ret = "(";
    QString current_action = wnd_->currentAction();
    if(!current_action.isEmpty())
	ret += QString("(current-action . %1)").arg(current_action);
    int current_step = wnd_->currentStep();
    ret += QString("(current-step . %1)").arg(current_step);
    ret += ")";
    return ret;
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
	wnd_->clearActions();
	QStringList data = value.split(";", QString::KeepEmptyParts);
	const int len = data.size();
	for(int i=0;i+2 < len;i+=3)
	    wnd_->addAction(data[i], data[i+1], data[i+2]);
    }
    else if( "action-add" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	const int len = data.size();
	if( len >= 3 )
	    wnd_->addAction(data[0], data[1], data[2]);
	else if( len >= 2 )
	    wnd_->addAction(data[0], data[1], "");
	else if( len >= 1 )
	    wnd_->addAction(data[0], "", "");
    }
    else if( "action-remove" == name )
    {
	wnd_->removeAction(value);
    }
    else if( "actions-clear" == name )
    {
	wnd_->clearSteps();
    }
    else if( "action-activity" == name )
    {
	QStringList data = value.split(";");
	if( data.size() >= 2 )
	{
	    wnd_->setActionActivity(data[0], "true" == data[1]);
	}
    }
    else if( "steps" == name )
    {
	wnd_->clearActions();
	QStringList data = value.split(";", QString::KeepEmptyParts);
	const int len = data.size();
	for(int i=0;i+1 < len;i+=2)
	    wnd_->addStep(data[i], data[i+1]);
    }
    else if( "step-add" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	const int len = data.size();
	if( len >= 2 )
	    wnd_->addStep(data[0], data[1]);
	else if( len >= 1 )
	    wnd_->addStep(data[0], "");
    }
    else if( "step-remove" == name )
    {
	wnd_->removeStep(value.toInt());
    }
    else if( "steps-clear" == name )
    {
	wnd_->clearSteps();
    }
    else if( "step-activity" == name )
    {
	QStringList data = value.split(";");
	if( data.size() >= 2 )
	{
	    wnd_->setStepActivity(data[0].toInt(), "true" == data[1]);
	}
    }
    else if ("current-step" == name)
    {
	int n = value.toInt();
	if( n >= 0)
	    wnd_->setCurrentStep(n);
    }
    else
	alWidget::setAttr(name,value);
}
