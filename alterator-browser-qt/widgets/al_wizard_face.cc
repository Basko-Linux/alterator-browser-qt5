
#include <QScrollArea>
#include <QHelpEvent>

#include "global.hh"
#include "widgets.hh"
#include "al_wizard_face.hh"
#include "a_pixmaps.hh"

extern alWizardFace *wizard_face;
extern Enums *enums;

// AWizardFace
AWizardFace::AWizardFace(QWidget *parent, Qt::WFlags f):
    QWidget(parent, f)
{

    current_step = 0;

    title_widget = new QFrame(this);
    title_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    //title_widget->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);

    title_icon = new QLabel(title_widget);
    title_icon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    //title_icon->setAlignment(Qt::AlignCenter);

    logo_icon = new QLabel(this);
    logo_icon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    logo_icon->setAlignment(Qt::AlignLeft);
    //logo_icon->setPixmap(getPixmap("theme:logo_32"));
    logo_icon->setPixmap(getPixmap("logo_48"));

    title_text = new QLabel(title_widget);
    title_text->setObjectName("title_text");
    title_text->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    title_text->setAlignment(Qt::AlignLeft);
    QFont title_text_font = title_text->font();
    title_text_font.setBold(true);
//    int title_text_font_px_size = (int)(title_text_font.pixelSize()*1.5);
    int title_text_font_pt_size = (int)(title_text_font.pointSize()*1.5);
//    title_text_font.setPixelSize(title_text_font_px_size);
    title_text_font.setPointSize(title_text_font_pt_size);
    title_text->setFont(title_text_font);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setBackgroundRole(QPalette::NoRole);
    scroll->viewport()->setBackgroundRole(QPalette::NoRole);
    //scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scroll->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    scroll->setWidgetResizable( true );

    view_widget = new QWidget(scroll->viewport());
    view_widget->setBackgroundRole(QPalette::NoRole);
    view_widget->setObjectName("view");

    scroll->setWidget(view_widget);

    bottom_widget = new QFrame(this);
    bottom_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    //bottom_widget->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);

    menu_btn = new QPushButton(translateActionText("Menu"), bottom_widget);
    menu_btn->hide();
    menu_btn->setIcon(QIcon(getPixmap("theme:up")));
    menu = new QMenu();
    menu_btn->setMenu(menu);

    title_layout = new QHBoxLayout( title_widget );
    title_layout->setMargin(0);
    title_layout->setSpacing(5);

    QHBoxLayout *bottom_layout = new QHBoxLayout(bottom_widget);
    bottom_layout->setMargin(0);
    bottom_layout->setSpacing(0);

    main_layout = new QGridLayout(this);
    main_layout->setMargin(5);
    main_layout->setSpacing(5);
    main_layout->addWidget( title_widget, 0, 0 );
    main_layout->addWidget( scroll, 1, 0);
    main_layout->addWidget( bottom_widget, 2, 0);

    menu_layout = new QHBoxLayout();
    menu_layout->setMargin(5);
    menu_layout->setSpacing(5);

    buttons_layout = new QHBoxLayout();
    buttons_layout->setMargin(5);
    buttons_layout->setSpacing(5);

    title_layout->insertStretch(0, 1);
    title_layout->insertWidget(1, title_icon, 0, Qt::AlignRight);
    title_layout->insertWidget(2, title_text, 0, Qt::AlignLeft);
    title_layout->insertStretch(3, 1);

    bottom_layout->addWidget(logo_icon, Qt::AlignLeft);
    bottom_layout->addLayout(menu_layout, Qt::AlignLeft);
    bottom_layout->addLayout(buttons_layout, Qt::AlignRight);

    menu_layout->insertWidget(0, menu_btn, 0, Qt::AlignLeft);
    menu_layout->insertStretch(-1, 1);
    buttons_layout->insertStretch(0, 1);

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

int AWizardFace::findButtonPosition(UserActionType type)
{
    int pos = -1;
    QMapIterator<QString, UserActionType> it(button_types);
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

int AWizardFace::newButtonPosition(UserActionType type)
{
    switch( type )
    {
	case UserActionApply:
	    {
		int pos = findButtonPosition( UserActionCancel );
		if( pos >= 0 )
		    return pos;
		else
		    pos = findButtonPosition( UserActionBackward );
		if( pos >= 0 )
		    return pos;
		else
		    return findButtonPosition( UserActionForward );
	    }
	case UserActionCancel:
	    {
		int pos = findButtonPosition( UserActionApply );
		if( pos >= 0 )
		    return ++pos;
		else
		    pos = findButtonPosition( UserActionBackward );
		if( pos >= 0 )
		    return pos;
		else
		    return findButtonPosition( UserActionForward );
	    }
	case UserActionBackward:
	    {
		int pos = findButtonPosition( UserActionForward );
		if( pos >= 0 )
		    return pos;
		else
		    return -1;
	    }
	case UserActionFinish:
	case UserActionForward:
	    {
		return -1;
	    }
	case UserActionAbort:
	    {
		return 1;
	    }
	case UserActionHelp:
	    {
		return 1;
	    }
	case UserActionGeneric:
	default:
	    {
		return 2;
	    }
    }
}

Qt::Alignment AWizardFace::newButtonAlignment(UserActionType type)
{
    switch( type )
    {
	case UserActionAbort:
	case UserActionHelp:
	    {
		return Qt::AlignLeft;
	    }
	case UserActionFinish:
	case UserActionApply:
	case UserActionCancel:
	case UserActionForward:
	case UserActionBackward:
	    {
		return Qt::AlignRight;
	    }
	case UserActionGeneric:
	default:
	    {
		return Qt::AlignCenter;
	    }
    }
}

QPixmap AWizardFace::defaultActionIcon(UserActionType type)
{
    QString name;
    switch( type )
    {
	case UserActionFinish:
	    {
		name = "theme:down";
		break;
	    }
	case UserActionAbort:
	    {
		name = "theme:cancel";
		break;
	    }
	case UserActionHelp:
	    {
		name = "theme:help";
		break;
	    }
	case UserActionApply:
	    {
		name = "theme:apply";
		break;
	    }
	case UserActionCancel:
	    {
		name = "theme:cancel";
		break;
	    }
	case UserActionForward:
	    {
		name = "theme:forward";
		break;
	    }
	case UserActionBackward:
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
	UserActionType type = enums->strToUserAction(key);
	addAction(key, type);
	if( !name.isEmpty() )
	    setActionText(key, name);
	if( !pixmap.isEmpty() )
	    setActionPixmap(key, pixmap);
    }
}


void AWizardFace::addAction(const QString &key, UserActionType type)
{
    switch( type )
    {
	case UserActionFinish:
	case UserActionApply:
	case UserActionCancel:
	case UserActionBackward:
	case UserActionForward:
	case UserActionHelp:
	case UserActionAbort:
	    {
		if( !buttons.contains(key) )
	        {
		    QBoxLayout *lay = buttons_layout;
		    if( type == UserActionHelp || type == UserActionAbort )
			lay = menu_layout;
		    QPushButton *b = new QPushButton(bottom_widget);
		    b->setIcon(QIcon(defaultActionIcon(type)));
		    lay->insertWidget( newButtonPosition(type), b, 0, newButtonAlignment(type) );
		    if( type == UserActionForward || type == UserActionFinish )
			b->setFocus();
		    buttons[key] = b;
		    button_types[key] = type;
		    connect(b, SIGNAL(clicked()), action_signal_mapper, SLOT(map()));
		    action_signal_mapper->setMapping(b, key);
		    break;
		}
	    }
	case UserActionGeneric:
	default:
	    {
		if( !menus.contains(key) )
		{
		    menu_btn->show();
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
        title_icon->setPixmap(getPixmap(item.first));
	title_text->setText(QString("%1/%2: %3").arg(n+1).arg(steps_n).arg(item.second));
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
    children += bottom_widget->findChildren<QAbstractButton*>();
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
    AlteratorRequestFlags flags = AlteratorRequestDefault;
    UserActionType type = enums->strToUserAction(key);
    if( type == UserActionHelp )
    {
	QHelpEvent *hlp = new QHelpEvent((QEvent::Type)EVENT_HELP, QPoint(), QPoint());
	QApplication::postEvent(main_window, hlp);
    }
    current_action = key;

    if( type == UserActionForward )
	flags = flags | AlteratorRequestTimeReset;
    switch( type )
    {
	case UserActionApply:
	case UserActionFinish:
	case UserActionForward:
	case UserActionBackward:
	{
	    flags = flags | AlteratorRequestBlocking;
	    emit blockingActionSelected(flags);
	    break;
	}
	default:
	    emit actionSelected();
    }
}

void AWizardFace::onSelectStep(QListWidgetItem*)
{
    emit stepSelected();
}

bool AWizardFace::onEnter()
{
    bool ret = false;
    UserActionType action;
    QString key;
    QAbstractButton *btn = 0;

    QMapIterator<QString, QAbstractButton*> it(buttons);
    while( it.hasNext() )
    {
	it.next();
	action = enums->strToUserAction(it.key());
	if( action == UserActionFinish
	    || action == UserActionForward )
	{
	    key = it.key();
	    btn = it.value();
	}
    }

    if( btn && btn->isVisible() && btn->isEnabled() && !key.isEmpty() )
    {
	onSelectAction(key);
	ret = true;
    }
    return ret;
}

#if 0
void AWizardFace::keyPressEvent(QKeyEvent* e)
{
    bool entered = false;
    switch( e->key() )
    {
	case Qt::Key_Return:
	case Qt::Key_Enter:
	{
	    entered = onEnter();
	    break;
	}
	default:
	    break;
    }
    if( !entered )
	 QWidget::keyPressEvent(e);
}
#endif

// alWizardFace
alWizardFace::alWizardFace(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation):
    alWidgetPre<AWizardFace>(attr,WWizardFace,id,parent)
{
    QBoxLayout *bl;
    Qt::Orientation o = Utils::fixOrientation(orientation, Qt::Vertical);
    if( o == Qt::Horizontal )
	bl = new QHBoxLayout(wnd_->getViewWidget());
    else
	bl = new QVBoxLayout(wnd_->getViewWidget());
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
    {
	connect(wnd_,SIGNAL(actionSelected()), SLOT(onClick()));
	connect(wnd_,SIGNAL(blockingActionSelected(const AlteratorRequestFlags)), SLOT(onBlockingClick(const AlteratorRequestFlags)));
    }
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
