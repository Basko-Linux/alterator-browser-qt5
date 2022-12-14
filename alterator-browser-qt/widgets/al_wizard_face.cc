
#include <QHelpEvent>
#include <QScrollBar>

#include "global.hh"
#include "widgets.hh"
#include "al_wizard_face.hh"
#include "a_pixmaps.hh"

// AWizardFaceStepListItem
AWizardFaceStepListItem::AWizardFaceStepListItem(QWidget *parent, const QPixmap &pxdone, const QPixmap &pxcurrent, const QPixmap &pxundone):
    QWidget(parent),
    pix_done(pxdone),
    pix_current(pxcurrent),
    pix_undone(pxundone)
{
    lbl_img = new QLabel(this);
    lbl_img->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    lbl_txt = new QLabel(this);
    lbl_txt->setAlignment(Qt::AlignLeft);

    QHBoxLayout *main_lay = new QHBoxLayout(this);
    main_lay->setMargin(0);
    main_lay->addWidget(lbl_img);
    main_lay->addWidget(lbl_txt);
}

AWizardFaceStepListItem::~AWizardFaceStepListItem()
{}

QString AWizardFaceStepListItem::AWizardFaceStepListItem::text()
{
    return lbl_txt->text();
}

void AWizardFaceStepListItem::setText(const QString &txt)
{
    lbl_txt->setText(txt);
}

void AWizardFaceStepListItem::setLookDone()
{
#if 0
    setEnabled(false);
    QFont font = lbl_txt->font();
    font.setBold(false);
    lbl_txt->setFont(font);
#endif
    lbl_img->setPixmap(pix_done);
}

void AWizardFaceStepListItem::setLookCurrent()
{
#if 0
    setEnabled(true);
    QFont font = lbl_txt->font();
    font.setBold(true);
    lbl_txt->setFont(font);
#endif
    lbl_img->setPixmap(pix_current);
}

void AWizardFaceStepListItem::setLookUndone()
{
#if 0
    setEnabled(true);
    QFont font = lbl_txt->font();
    font.setBold(false);
    lbl_txt->setFont(font);
#endif
    lbl_img->setPixmap(pix_undone);
}


// AWizardFaceStepList
AWizardFaceStepList::AWizardFaceStepList(QWidget *parent):
    QFrame(parent)
{
    current = -1;

    setSizePolicy(QSizePolicy::Maximum, QSizePolicy::MinimumExpanding);
    setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);

    QVBoxLayout *mlay = new QVBoxLayout(this);
    QHBoxLayout *toplay = new QHBoxLayout();
    lay = new QVBoxLayout();
    QVBoxLayout *bottomlay = new QVBoxLayout();
    mlay->addLayout(toplay);
    mlay->addLayout(lay);
    mlay->addLayout(bottomlay);
    bottomlay->addStretch(1);

    logo_icon = new LogoIcon(this, true);
    logo_icon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    toplay->addWidget(logo_icon, Qt::AlignHCenter);

    QPixmap pix_unknown = getPixmap("theme:unknown");
    pix_done = getPixmap("wzface-step-done");
    if( pix_done.toImage() == pix_unknown.toImage() )
	pix_done = getPixmap("theme:check-on");
    pix_current = getPixmap("wzface-step-current");
    if( pix_current.toImage() == pix_unknown.toImage() )
	pix_current = getPixmap("theme:check-off");
    pix_undone = getPixmap("theme:null");
    QPixmap logo_icon_pix = getPixmap("logo_width");
    if( logo_icon_pix.toImage() == pix_unknown.toImage() )
	logo_icon_pix = getPixmap("logo_48");
    if( logo_icon_pix.toImage() != pix_unknown.toImage() )
	logo_icon->setPixmap(logo_icon_pix);
}

AWizardFaceStepList::~AWizardFaceStepList()
{}

QWidget* AWizardFaceStepList::logo()
{
    return logo_icon;
}

void AWizardFaceStepList::setCurrent(int new_current)
{
    if( new_current == current )
	return;

    if( new_current > current )
    {
	int i = -1;
	QListIterator<AWizardFaceListItemPriv> it(lst);
	while( it.hasNext() )
	{
	    i++;
	    AWizardFaceListItemPriv litem = it.next();
	    if( i < current )
		continue;
	    else if( i >= current && i < new_current )
		litem.second->setLookDone();
	    else if( i == new_current )
		litem.second->setLookCurrent();
	    else
		break;
	}
    }
    else
    {
	int i = lst.size();
	QListIterator<AWizardFaceListItemPriv> it(lst);
	it.toBack();
	while( it.hasPrevious() )
	{
	    i--;
	    AWizardFaceListItemPriv litem = it.previous();
	    if( i > current )
		continue;
	    else if( i <= current && i > new_current )
		litem.second->setLookUndone();
	    else if( i == new_current )
		litem.second->setLookCurrent();
	    else
		break;
	}
    }
    current = new_current;
}

void AWizardFaceStepList::append(QPair<QString, QString> item)
{
    AWizardFaceStepListItem *witem = new AWizardFaceStepListItem(this, pix_done, pix_current, pix_undone);
    witem->setText(item.second);
    witem->setLookUndone();
    lay->addWidget(witem);
    lst.append(qMakePair(item.first,witem));
}

int AWizardFaceStepList::size()
{
    return lst.size();
}

QPair<QString,QString> AWizardFaceStepList::value(int n)
{
    AWizardFaceStepListItem *wdummy = 0;
    QString second;
    AWizardFaceListItemPriv litem = lst.value(n, qMakePair(QString(), wdummy));
    if( litem.second )
	second = litem.second->text();
    return qMakePair(litem.first,second);
}

void AWizardFaceStepList::removeAt(int n)
{
    AWizardFaceStepListItem *wdummy = 0;
    AWizardFaceListItemPriv litem = lst.value(n, qMakePair(QString(), wdummy));
    lst.removeAt(n);
    if( litem.second )
    {
	delete litem.second;
	litem.second = 0;
    }
    if( lst.isEmpty() )
	current = -1;
}

void AWizardFaceStepList::clear()
{
    Q_FOREACH( AWizardFaceListItemPriv litem, lst)
    {
	if( litem.second )
	{
	    delete litem.second;
	    litem.second = 0;
	}
    }
    lst.clear();
    current = -1;
}

void AWizardFaceStepList::replace(int n, QPair<QString, QString> item)
{
    if( n > 0 && n < lst.size() )
    {
	AWizardFaceStepListItem *wdummy = 0;
	QPair<QString,AWizardFaceStepListItem*> litem = lst.value(n, qMakePair(QString(), wdummy));
	if( litem.second )
	{
	    litem.second->setText(item.second);
	    lst.replace(n, qMakePair(item.first, litem.second));
	}
    }
}

// AWizardFace
AWizardFace::AWizardFace(QWidget *parent, const Qt::Orientation):
    AWidget<AWizardFaceBase>(parent)
{
    setObjectName("wizardface");
    setLineWidth(0);
    setFrameStyle(QFrame::Plain | QFrame::NoFrame);

    current_step = -1;
    current_action = "__undefined__";
    has_logo_icon_pix = false;

    steplist = new AWizardFaceStepList(this);
    steplist->setVisible(false);
    connect(steplist->logo(), SIGNAL(clicked()), this, SLOT(onStepListSwitchVisibility()));

    title_widget = new QFrame(this);
    title_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    //title_widget->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);

    title_icon = new QLabel(title_widget);
    title_icon->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    //title_icon->setAlignment(Qt::AlignCenter);

    title_text = new QLabel(title_widget);
    title_text->setObjectName("wizardface_title_text");
    title_text->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    title_text->setAlignment(Qt::AlignLeft);
    QFont title_text_font = title_text->font();
    title_text_font.setBold(true);
    int title_text_font_pt_size = (int)(title_text_font.pointSize()*1.3);
    title_text_font.setPointSize(title_text_font_pt_size);
    title_text->setFont(title_text_font);

    scroll = new QScrollArea(this);
    scroll->setFocusPolicy(Qt::NoFocus);
    { // install event filter for scroll
	QScrollBar *vs = scroll->verticalScrollBar();
	if( vs )
	    vs->installEventFilter(this);
	QScrollBar *hs = scroll->horizontalScrollBar();
	if( hs )
	    hs->installEventFilter(this);
    }
    scroll->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    scroll->setWidgetResizable( true );
    QPalette scr_palette = scroll->palette();
    scr_palette.setBrush(QPalette::Window, QBrush(QColor(0,0,0,0)));
    scroll->setPalette(scr_palette);

    view_widget = new QWidget(scroll->viewport());
    view_widget->setObjectName("view");
    QPalette vw_palette = view_widget->palette();
    vw_palette.setBrush(QPalette::Window, QBrush(QColor(0,0,0,0)));
    view_widget->setPalette(vw_palette);

    scroll->setWidget(view_widget);

    bottom_widget = new QFrame(this);
    bottom_widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Maximum);
    //bottom_widget->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);

    logo_icon = new LogoIcon(bottom_widget, true);
    logo_icon->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    connect(logo_icon, SIGNAL(clicked()), this, SLOT(onStepListSwitchVisibility()));

    //menu_btn = new QPushButton(tr("Menu"), bottom_widget);
    menu_btn = new QPushButton(bottom_widget);
    menu_btn->setVisible(false);
    menu_btn->setIcon(QIcon(getPixmap("theme:up")));
    menu = new QMenu();
    menu_btn->setMenu(menu);
    menu_act_steplist = menu->addAction(tr("Show steps list"));
    connect(menu_act_steplist, SIGNAL(triggered(bool)), this, SLOT(onStepListSwitchVisibility(bool)));

    QPixmap pix_unknown = getPixmap("theme:unknown");
    QPixmap logo_icon_pix = getPixmap("logo_48");
    if( logo_icon_pix.toImage() != pix_unknown.toImage() )
    {
	logo_icon->setPixmap(logo_icon_pix);
	has_logo_icon_pix = true;
    }
    else
    {
	menu_btn->setVisible(true);
	logo_icon->setVisible(false);
    }

    title_layout = new QHBoxLayout( title_widget );
    title_layout->setMargin(0);
    title_layout->setSpacing(5);

    QHBoxLayout *bottom_layout = new QHBoxLayout(bottom_widget);
    bottom_layout->setMargin(0);
    bottom_layout->setSpacing(5);

    QHBoxLayout *main_layout = new QHBoxLayout(this);
    main_layout->setMargin(0);
    QVBoxLayout *left_layout = new QVBoxLayout();
    left_layout->setMargin(0);
    QVBoxLayout *right_layout = new QVBoxLayout();
    right_layout->setMargin(10);
    right_layout->setSpacing(10);

    main_layout->addLayout(left_layout);
    main_layout->addLayout(right_layout);
    left_layout->addWidget(steplist);
    right_layout->addWidget(title_widget);
    right_layout->addWidget(scroll);
    right_layout->addWidget(bottom_widget);

    menu_layout = new QHBoxLayout();
    menu_layout->setMargin(0);
    menu_layout->setSpacing(5);

    buttons_layout = new QHBoxLayout();
    buttons_layout->setMargin(0);
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

bool AWizardFace::eventFilter(QObject *o, QEvent *e)
{
    if( e->type() == QEvent::Show )
    {
	QScrollBar *hs = scroll->horizontalScrollBar();
	QScrollBar *vs = scroll->verticalScrollBar();
	if( (o == hs || o == vs) && scroll->focusPolicy() == Qt::NoFocus )
	{
	    scroll->setFocusPolicy(Qt::StrongFocus);
	}
    }
    else if( e->type() == QEvent::Hide )
    {
	QScrollBar *hs = scroll->horizontalScrollBar();
	QScrollBar *vs = scroll->verticalScrollBar();
	if( ((o == hs && !vs->isVisible()) || ( o == vs && !hs->isVisible() )) && scroll->focusPolicy() != Qt::NoFocus )
	{
	    scroll->setFocusPolicy(Qt::NoFocus);
	    if(scroll->hasFocus())
	    {
		QKeyEvent *k = new QKeyEvent(QEvent::KeyPress, Qt::Key_Tab, Qt::NoModifier);
		QCoreApplication::postEvent(scroll, k);
	    }
	}
    }

    return AWizardFaceBase::eventFilter(o, e);
}

int AWizardFace::findButtonPosition(UserActionType type)
{
    int pos = buttons.count() - 1;
    QMapIterator<QString, UserActionType> it(action_types);
    while( it.hasNext() )
    {
	it.next();
	if( it.value() == type && buttons.contains(it.key()) )
	{
	    pos = buttons_layout->indexOf( buttons[it.key()] );
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
	UserActionType type = g_enums->strToUserAction(key.toLatin1());
	addAction(key, type);
	if( !name.isEmpty() )
	    setActionText(key, name);
	if( !pixmap.isEmpty() )
	    setActionPixmap(key, pixmap);
	switch( type )
	{
	    case UserActionFinish:
	    case UserActionForward:
	    {
		setActionToolTip(key);
		break;
	    }
	    default:
		break;
	}
    }
}


void AWizardFace::addAction(const QString &key, UserActionType type)
{
    if( buttons.contains(key) || menus.contains(key) )
	return;

    switch( type )
    {
	case UserActionFinish:
	case UserActionApply:
	case UserActionCancel:
	case UserActionBackward:
	case UserActionForward:
	    {
		    addActionButton(key, type);
		    break;
	    }
	case UserActionHelp:
	case UserActionAbort:
	case UserActionGeneric:
	default:
	    {
		if( !menu_btn->isVisible() || bottom_widget->findChildren<QPushButton*>().count() >= 4 )
		{
		    // move additional buttons to menu
		    QMapIterator<QString, QAbstractButton*> it(buttons);
		    while(it.hasNext())
		    {
			QAbstractButton *btn = it.next().value();
			if( menu_layout->indexOf(btn) >= 0 )
			{
			    QString key = it.key();
			    QString text = btn->text();
			    QIcon icon = btn->icon();
			    UserActionType tp = action_types[key];
			    removeAction(key);
			    addActionMenu(key, tp);
			    setActionText(key, text);
			    setActionIcon(key, icon);
			}
		    }
		}

		if( menu_btn->isVisible() ) {
		    addActionMenu(key, type);
		} else {
		    addActionButton(key, type);
		}
		break;
	    }
    }
}

void AWizardFace::addActionButton(const QString &key, UserActionType type)
{
    QBoxLayout *lay;
    switch( type )
    {
	case UserActionHelp:
	case UserActionAbort:
	{
	    lay = menu_layout;
	    break;
	}
	default:
	    lay = buttons_layout;
    }
    QPushButton *b = new QPushButton(bottom_widget);
    b->setIcon(QIcon(defaultActionIcon(type)));
    lay->insertWidget( newButtonPosition(type), b, 0, newButtonAlignment(type) );
    if( type == UserActionForward || type == UserActionFinish )
	b->setFocus();
    buttons[key] = b;
    action_types[key] = type;
    connect(b, SIGNAL(clicked()), action_signal_mapper, SLOT(map()));
    action_signal_mapper->setMapping(b, key);
}

void AWizardFace::addActionMenu(const QString &key, UserActionType type)
{
    menu_btn->show();
    QAction *a = menu->addAction("");
    a->setIcon(QIcon(defaultActionIcon(type)));
    menus[key] = a;
    action_types[key] = type;
    connect(a, SIGNAL(triggered()), action_signal_mapper, SLOT(map()));
    action_signal_mapper->setMapping(a, key);
}

void moveMenuButtonsToMenu()
{
}

void AWizardFace::removeAction(const QString &key)
{
    if( buttons.contains(key) )
    {
	QAbstractButton *b = buttons.take(key);
	if(b)
	{
	    action_signal_mapper->removeMappings(b);
	    action_types.remove(key);
	    b->deleteLater();
	}
    }
    else if( menus.contains(key) )
    {
	QAction *a = menus[key];
	action_signal_mapper->removeMappings(a);
	action_types.remove(key);
	menu->removeAction(a);
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
    action_types.clear();
    menu->clear();
    menus.clear();
}

void AWizardFace::addStep(const QString& name, const QString& pixmap)
{
    //qDebug("AWizardFace::addStep(%s,%s)", name.toLatin1().data(), pixmap.toLatin1().data() );
    steplist->append(QPair<QString, QString>(pixmap, name));
}

void AWizardFace::removeStep(int n)
{
    steplist->removeAt(n);
}

void AWizardFace::clearSteps()
{
    steplist->clear();
}

QWidget* AWizardFace::getViewWidget()
{
    return view_widget;
}

void AWizardFace::setActionToolTip(const QString &key, const QString &txt_in)
{
    QAbstractButton *b = 0;
    QAction *a = 0;
    QByteArray tmpl_tooltip;

    if(txt_in.isEmpty())
    {
	switch( action_types[key] )
	{
	    case UserActionFinish:
	    case UserActionForward:
	    {
		tmpl_tooltip = QT_TRANSLATE_NOOP("AWizardFace", "%1: press F12 or Enter");
		break;
	    }
	    default:
		break;
	}
    }

    if( buttons.contains(key) )
    {
	b = buttons[key];
    }
    else if( menus.contains(key) )
    {
	a = menus[key];
    }

    if( b ) {
	if( !tmpl_tooltip.isEmpty() ) {
	    b->setToolTip(tr(tmpl_tooltip).arg(b->text()));
	    //qWarning("AWizardFace add button");
	    connect(g_browser, &Browser::languageChanged, b, [b, tmpl_tooltip](){
		//qWarning("AWizardFace TR");
		b->setToolTip(QCoreApplication::translate("AWizardFace", tmpl_tooltip).arg(b->text()));
	    }, Qt::QueuedConnection);
	} else {
	    b->setToolTip(txt_in);
	}
    } else if( a ) {
	if( !tmpl_tooltip.isEmpty() ) {
	    a->setToolTip(tr(tmpl_tooltip).arg(a->text()));
	    connect(g_browser, &Browser::languageChanged, a, [a, tmpl_tooltip](){
		a->setToolTip(QCoreApplication::translate("AWizardFace", tmpl_tooltip).arg(a->text()));
	    }, Qt::QueuedConnection);
	} else {
	    a->setToolTip(txt_in);
	}
    }
}

void AWizardFace::setActionText(const QString &key, const QString &value)
{
    if( buttons.contains(key) )
	buttons[key]->setText(value);
    else if( menus.contains(key) )
	menus[key]->setText(value);
}

void AWizardFace::setActionPixmap(const QString &key, const QString &value)
{
    if( buttons.contains(key) )
	buttons[key]->setIcon(QIcon(getPixmap(value)));
    else if ( menus.contains(key) )
	menus[key]->setIcon(QIcon(getPixmap(value)));
}

void AWizardFace::setActionIcon(const QString &key, const QIcon &icon)
{
    if( buttons.contains(key) )
	buttons[key]->setIcon(icon);
    else if ( menus.contains(key) )
	menus[key]->setIcon(icon);
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
    if( n < steplist->size() )
    {
	QPair<QString, QString> item = steplist->value(n);
	item.second = value;
	steplist->replace(n, item);
    }
}

void AWizardFace::setStepPixmap(int n, const QString &value)
{
    if( n < steplist->size() )
    {
	QPair<QString, QString> item = steplist->value(n);
	item.first = value;
	steplist->replace(n, item);
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
}

void AWizardFace::setCurrentStep( int n )
{
    int steps_n = steplist->size();
    if( n < steps_n )
    {
	QPair<QString, QString> item = steplist->value(n);
        title_icon->setPixmap(getPixmap(item.first));
	title_text->setText(QString("%1/%2: %3").arg(n+1).arg(steps_n).arg(item.second));
	steplist->setCurrent(n);
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
    UserActionType type = g_enums->strToUserAction(key.toLatin1());
    if( type == UserActionHelp )
	g_browser->showHelp();

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
	    Q_EMIT blockingActionSelected(flags);
	    break;
	}
	default:
	    Q_EMIT actionSelected();
    }
}

void AWizardFace::onSelectStep(QListWidgetItem*)
{
    Q_EMIT stepSelected();
}

bool AWizardFace::onEnter()
{
    bool ret = false;
    QString key;
    QAbstractButton *btn = 0;

    bool break_while = false;
    QMapIterator<QString, QAbstractButton*> it(buttons);
    while( it.hasNext() && !break_while )
    {
	it.next();
	switch( g_enums->strToUserAction(it.key().toLatin1()) )
	{
	    case UserActionFinish:
	    case UserActionForward:
	    {
		btn = it.value();
		key = it.key();
		if( btn->hasFocus() )
		    break_while = true;
		break;
	    }
	    case UserActionBackward:
	    {
		QAbstractButton *btn_backward = it.value();
		if( btn_backward->hasFocus() )
		{
		    btn = btn_backward;
		    key = it.key();
		    break_while = true;
		}
		break;
	    }
	    default:
		break;
	}
    }

    if( btn && btn->isVisible() && btn->isEnabled() && !key.isEmpty() )
    {
	onSelectAction(key);
	ret = true;
    }
    return ret;
}

void AWizardFace::keyPressEvent(QKeyEvent* e)
{
    bool entered = false;
    switch( e->key() )
    {
	case Qt::Key_Return:
	case Qt::Key_Enter:
	case Qt::Key_F12:
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

void AWizardFace::onStepListSwitchVisibility()
{
    onStepListSwitchVisibility(true);
}

void AWizardFace::onStepListSwitchVisibility(bool)
{
    bool sl_visible = !steplist->isVisible();
    steplist->setVisible(sl_visible);
    if( sl_visible )
	menu_act_steplist->setText(tr("Hide steps list"));
    else
	menu_act_steplist->setText(tr("Show steps list"));
    QLayoutItem *stretch0 = title_layout->itemAt(0);
    if( stretch0->spacerItem() )
    {
	title_layout->removeItem(stretch0);
	if( sl_visible )
	    title_layout->insertStretch(0, 0);
	else
	    title_layout->insertStretch(0, 1);
    }
    if( has_logo_icon_pix )
	logo_icon->setVisible(!sl_visible);
}

// alWizardFace
alWizardFace::alWizardFace(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
    alWidgetPre<AWizardFace>(attr,WWizardFace,id,parent)
{
    QBoxLayout *bl;
    Qt::Orientation o = Utils::fixOrientation(attr.value("orientation").o, Qt::Vertical);
    if( o == Qt::Horizontal ) {
	bl = new QHBoxLayout(wnd_->getViewWidget());
    } else {
	bl = new QVBoxLayout(wnd_->getViewWidget());
    }
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
	connect(wnd_,SIGNAL(blockingActionSelected(const AlteratorRequestFlags)), SLOT(onSpecialClick(const AlteratorRequestFlags)));
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
    if( "title" == name )
    {
	wnd_->setTitle(value);
	alWidget::setAttr(name,value);
    }
    else if( "actions" == name )
    {
	wnd_->clearActions();
	QStringList data = value.split(QLatin1String(";"), QT_KEEPEMPTYPARTS);
	QStringListIterator it(data);
	Q_FOREVER
	{
	    QString key, name, pixmap;
	    if(it.hasNext())
		key = it.next();
	    else
		break;
	    if(it.hasNext())
		name = it.next();
	    else
		break;
	    if(it.hasNext())
		pixmap = it.next();
	    else
		break;
	    wnd_->addAction(key, name, pixmap);
	}
    }
    else if( "action-add" == name )
    {
	QStringList data = value.split(QLatin1String(";"), QT_KEEPEMPTYPARTS);
	QStringListIterator it(data);
	if( data.size() > 0 )
	{
	    QString key, name, pixmap;
	    if(it.hasNext())
		key = it.next();
	    if(it.hasNext())
		name = it.next();
	    if(it.hasNext())
		pixmap = it.next();
	    wnd_->addAction(key, name, pixmap);
	}
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
	QStringList data = value.split(QLatin1String(";"));
	if( data.size() >= 2 )
	{
	    wnd_->setActionActivity(data[0], "true" == data[1]);
	}
    }
    else if( "action-text" == name )
    {
	QStringList data = value.split(QLatin1String(";"), QT_KEEPEMPTYPARTS);
	const int len = data.size();
	if( len >= 2 )
	    wnd_->setActionText(data[0], data[1]);
    }
    else if( "action-pixmap" == name )
    {
	QStringList data = value.split(QLatin1String(";"), QT_KEEPEMPTYPARTS);
	const int len = data.size();
	if( len >= 2 )
	    wnd_->setActionPixmap(data[0], data[1]);
    }
    else if( "steps" == name )
    {
	wnd_->clearSteps();
	QStringList data = value.split(QLatin1String(";"), QT_KEEPEMPTYPARTS);
	const int len = data.size();
	for(int i=0;i+1 < len;i+=2)
	    wnd_->addStep(data[i], data[i+1]);
    }
    else if( "step-add" == name )
    {
	QStringList data = value.split(QLatin1String(";"), QT_KEEPEMPTYPARTS);
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
    else if( "step-text" == name )
    {
	QStringList data = value.split(QLatin1String(";"), QT_KEEPEMPTYPARTS);
	const int len = data.size();
	if( len >= 2 )
	{
	    wnd_->setStepText(data[0].toInt(), data[1]);
	}
    }
    else if( "step-pixmap" == name )
    {
	QStringList data = value.split(QLatin1String(";"), QT_KEEPEMPTYPARTS);
	const int len = data.size();
	if( len >= 2 )
	{
	    wnd_->setStepPixmap(data[0].toInt(), data[1]);
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
