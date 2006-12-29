
#include <QScrollArea>
#include <QHelpEvent>

#include "global.hh"
#include "widgets.hh"
#include "al_wizard_face.hh"
#include "a_pixmaps.hh"

extern alWizardFace *wizard_face;

// AWizardFace
AWizardFace::AWizardFace(QWidget *parent, Qt::WFlags f):
    QDialog(parent, f)
{
    key2type["abort"]    = AWizardFace::ActionAbort;
    key2type["finish"]   = AWizardFace::ActionFinish;
    key2type["help"]     = AWizardFace::ActionHelp;
    key2type["apply"]    = AWizardFace::ActionApply;
    key2type["cancel"]   = AWizardFace::ActionCancel;
    key2type["backward"] = AWizardFace::ActionBackward;
    key2type["forward"]  = AWizardFace::ActionForward;
    key2type["generic"]  = AWizardFace::ActionGeneric;

    current_step = 0;


    title_widget = new QFrame(this);
    title_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    title_widget->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);

    title_icon = new QLabel(title_widget);
    title_icon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    title_icon->setAlignment(Qt::AlignLeft);

    title_text = new QLabel(title_widget);
    title_text->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    title_text->setAlignment(Qt::AlignCenter);
    QFont title_text_font = title_text->font();
    title_text_font.setBold(true);
//    int title_text_font_px_size = (int)(title_text_font.pixelSize()*1.5);
    int title_text_font_pt_size = (int)(title_text_font.pointSize()*1.5);
//    title_text_font.setPixelSize(title_text_font_px_size);
    title_text_font.setPointSize(title_text_font_pt_size);
    title_text->setFont(title_text_font);

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
    menu_btn->setIcon(QIcon(getPixmap("theme:up")));
    menu = new QMenu();
    menu_btn->setMenu(menu);

    title_layout = new QHBoxLayout( title_widget );
    buttons_layout = new QHBoxLayout( buttons_widget );
    main_layout = new QGridLayout(this);
    main_layout->addWidget( title_widget, 0, 0 );
    main_layout->addWidget( scroll, 1, 0);
    main_layout->addWidget( buttons_widget, 2, 0);

    title_layout->insertWidget(0, title_icon, 0, Qt::AlignLeft);
    title_layout->addWidget( title_text );
    buttons_layout->insertWidget(0, menu_btn, 0, Qt::AlignLeft);

    action_signal_mapper = new QSignalMapper(this);
    connect(action_signal_mapper, SIGNAL(mapped(const QString &)),
	this, SIGNAL(actionSelected(const QString &)));
    connect( this, SIGNAL(actionSelected(const QString&)),
	this, SLOT(onSelectAction(const QString&)) );
    //connect( stepbox, SIGNAL(itemActivated(QListWidgetItem*)),
    //	this, SLOT(onSelectStep(QListWidgetItem*)) );
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
	case ActionFinish:
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
	case ActionFinish:
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
		name = "theme:down"; // names are fake until fixed
		break;
	    }
	case ActionAbort:
	    {
		name = "theme:cancel";
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
	if( !name.isEmpty() )
	    setActionText(key, name);
	if( !pixmap.isEmpty() )
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
		if( !buttons.contains(key) )
	        {
		    QPushButton *b = new QPushButton(buttons_widget);
		    b->setIcon(QIcon(defaultActionIcon(type)));
		    buttons_layout->insertWidget( newButtonPosition(type), b, 0, newButtonAlignment(type) );
		    if( type == AWizardFace::ActionForward || type == AWizardFace::ActionFinish )
			b->setFocus();
		    buttons[key] = b;
		    button_types[key] = type;
		    connect(b, SIGNAL(clicked()), action_signal_mapper, SLOT(map()));
		    action_signal_mapper->setMapping(b, key);
		    break;
		}
	    }
	case ActionHelp:
	case ActionAbort:
	case ActionGeneric:
	default:
	    {
		if( !menus.contains(key) )
		{
		    QAction *a = menu->addAction("");
		    a->setIcon(QIcon(defaultActionIcon(type)));
		    menus[key] = a;
		    connect(a, SIGNAL(triggered()), action_signal_mapper, SLOT(map()));
		    action_signal_mapper->setMapping(a, key);
		    break;
		}
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
    //qDebug("AWizardFace::addStep(%s,%s)", name.toLatin1().data(), pixmap.toLatin1().data() );
    steplist.push_back(QPair<QString, QString>(pixmap, name));
}

void AWizardFace::removeStep(int n)
{
    steplist.removeAt(n);
}

void AWizardFace::clearSteps()
{
    steplist.clear();    
}

QWidget* AWizardFace::getViewWidget()
{
    return view_widget;
}

QString AWizardFace::translateActionText(const QString& txt)
{
    QString i18n_text = QApplication::translate("QDialogButtonBox", qPrintable(txt), 0, QApplication::UnicodeUTF8);
    return QApplication::translate("QAccel", qPrintable(i18n_text), 0, QApplication::UnicodeUTF8);
}

void AWizardFace::setActionText(const QString &key, const QString &value)
{
    if( buttons.contains(key) )
	buttons[key]->setText(translateActionText(value));
    else if( menus.contains(key) )
	menus[key]->setText(translateActionText(value));
}

void AWizardFace::setActionPixmap(const QString &key, const QString &value)
{
    if( buttons.contains(key) )
	buttons[key]->setIcon(QIcon(getPixmap(value)));
    else if ( menus.contains(key) )
	menus[key]->setIcon(QIcon(getPixmap(value)));
}

void AWizardFace::setActionActivity(const QString &key, bool enable)
{
    if( buttons.contains(key) )
	buttons[key]->setEnabled(enable);
    else if( menus.contains(key) )
	menus[key]->setEnabled(enable);
}

void AWizardFace::setStepText(int n, const QString &value)
{
    if( n < steplist.size() )
    {
	QPair<QString, QString> item = steplist.value(n);
	item.second = value;
	steplist.replace(n, item);
    }
}

void AWizardFace::setStepPixmap(int n, const QString &value)
{
    if( n < steplist.size() )
    {
	QPair<QString, QString> item = steplist.value(n);
	item.first = value;
	steplist.replace(n, item);
    }
}

void AWizardFace::setStepActivity(int n, bool a)
{
    /*
    QListWidgetItem *item = stepbox->item(n);
    if( item )
    {
	if( a )
	    item->setFlags(Qt::ItemIsEnabled);
	else
	    item->setFlags(item->flags() & ~Qt::ItemIsEnabled);
    }
    */
}

void AWizardFace::setTitle( const QString &value)
{
    title_text->setText(value);
    topLevelWidget()->setWindowTitle(value);
}

void AWizardFace::setCurrentStep( int n )
{
    int steps_n = steplist.size();
    if( n < steps_n )
    {
	QPair<QString, QString> item = steplist.value(n, QPair<QString, QString>("",""));
#if 0
        title_icon->setPixmap(getPixmap(item.first));
#else
        title_icon->setPixmap(getPixmap("theme:logo_32"));
#endif
	title_text->setText(QString("%1 (%2/%3)").arg(item.second).arg(n+1).arg(steps_n));
	current_step = n;
    }
}

int AWizardFace::currentStep()
{
    return current_step;
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

void AWizardFace::onSelectAction(const QString& key)
{
    //qDebug("current action is <%s>", key.toLatin1().data());
    if( key2type[key] == ActionHelp )
    {
	QHelpEvent *hlp = new QHelpEvent((QEvent::Type)EVENT_HELP, QPoint(), QPoint());
	QApplication::postEvent(main_window, hlp);
    }
    current_action = key;
    emit actionSelected();
}

void AWizardFace::onSelectStep(QListWidgetItem*)
{
    emit stepSelected();
}

void AWizardFace::onEnter()
{
    AWizardFace::ActionType action;
    QString key;
    QAbstractButton *btn = 0;

    QMapIterator<QString, QAbstractButton*> it(buttons);
    while( it.hasNext() )
    {
	it.next();
	action = key2type[it.key()];
	if( action == AWizardFace::ActionFinish
	    || action == AWizardFace::ActionForward )
	{
	    key = it.key();
	    btn = it.value();
	}
    }

    if( btn && btn->isVisible() && btn->isEnabled() && !key.isEmpty() )
    {
	onSelectAction(key);
    }
}

#if 0
void AWizardFace::keyPressEvent(QKeyEvent* e)
{
    switch( e->key() )
    {
	case Qt::Key_Return:
	case Qt::Key_Enter:
	{
	    onEnter();
	    break;
	}
	default:
	{
	    QWidget::keyPressEvent(e);
	    break;
	}
    }
}
#endif

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
    QString ret;
    QString current_action = wnd_->currentAction();
    if(!current_action.isEmpty())
	ret += QString(" (current-action . %1)").arg(current_action);
    int current_step = wnd_->currentStep();
    ret += QString(" (current-step . %1)").arg(current_step);
    return ret;
}

void alWizardFace::setAttr(const QString& name,const QString& value)
{
    /*
    if( "title" == name )
    {
	// set and continue
	wnd_->setTitle(value);
    }
    */

    if( "actions" == name )
    {
	wnd_->clearActions();
	wnd_->addAction("help", "Help", "");
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
	wnd_->clearActions();
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
    else if ("current-step" == name)
    {
	int n = value.toInt();
	if( n >= 0)
	    wnd_->setCurrentStep(n);
    }
    else
	alWidget::setAttr(name,value);
}
