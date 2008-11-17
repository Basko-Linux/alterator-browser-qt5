
#include <QScrollArea>
#include <QScrollBar>

#include "global.hh"
#include "al_center_face.hh"
#include "a_pixmaps.hh"

ACenterModuleButton::ACenterModuleButton(QWidget *parent):
    QLabel(parent)
{
    setTextFormat(Qt::RichText);
    setWordWrap(false);
    connect(this, SIGNAL(linkActivated(const QString&)), this, SLOT(onLinkActivated(const QString&)));
}

ACenterModuleButton::~ACenterModuleButton()
{}

void ACenterModuleButton::onLinkActivated(const QString&)
{
    emit clicked();
}

void ACenterModuleButton::setText(const QString &txt)
{
    QLabel::setText("<a href=\"/\">" + txt + "</a>");
}

void ACenterModuleButton::setPixmap(const QPixmap&)
{}

void ACenterModuleButton::setIcon(const QIcon&)
{}

/* CenterSectionModulesList */
ACenterSectionModulesList::ACenterSectionModulesList(QWidget *parent):
    QWidget(parent)
{
    lay = new FlowLayout(this);
    QFont items_font = font();
    int items_spacing = qMax(items_font.pixelSize(), items_font.pointSize());
    lay->setSpacing(items_spacing);

    signal_mapper = new QSignalMapper(this);
    connect(signal_mapper, SIGNAL(mapped(QWidget*)),
	this, SLOT(onItemClicked(QWidget*)));
}

ACenterSectionModulesList::~ACenterSectionModulesList()
{}

void ACenterSectionModulesList::onItemClicked(QWidget *w)
{
    ACenterSectionModulesListItem *i = qobject_cast<ACenterSectionModulesListItem*>(w);
    if(i)
	emit itemClicked(i);
}

void ACenterSectionModulesList::addItem(ACenterSectionModulesListItem *i)
{
    i->setParent(this);
    lay->addWidget(i);

    items.append(i);
    connect(i, SIGNAL(clicked()), signal_mapper, SLOT(map()));
    signal_mapper->setMapping(i, i);
}

void ACenterSectionModulesList::removeItem(ACenterSectionModulesListItem *i)
{
    int idx = items.indexOf(i);
    if( idx >= 0 )
    {
	signal_mapper->removeMappings(i);
	delete items.takeAt(idx);
    }
}

void ACenterSectionModulesList::setItemText(ACenterSectionModulesListItem *i, const QString& txt)
{
    if( items.contains(i) )
	i->setText(txt);
}

void ACenterSectionModulesList::setItemIcon(ACenterSectionModulesListItem *i, const QIcon& ico)
{
    if( items.contains(i) )
	i->setIcon(ico);
}

int ACenterSectionModulesList::count()
{
    return items.count();
}

bool ACenterSectionModulesList::isOwnerOfItem(ACenterSectionModulesListItem *i)
{
    return i->parentWidget() == this;
}

/* CenterSection */
ACenterSection::ACenterSection(QWidget *parent, const QString &title_text):
    QWidget(parent)
{
    setObjectName("module_section");

    pixmap = new QLabel(this);
    pixmap->setAlignment(Qt::AlignTop);

    title = new QLabel(title_text, this);
    QSizePolicy sp( QSizePolicy::Minimum, QSizePolicy::Maximum );
    sp.setHeightForWidth( title->sizePolicy().hasHeightForWidth() );
    title->setSizePolicy(sp);
    QFont title_font( title->font() );
    title_font.setBold( true );
    int title_font_pt_size = (int)(title_font.pointSize()*1.5);
    int title_font_px_size = (int)(title_font.pixelSize()*1.5);
    if( title_font_pt_size > title_font_px_size )
	title_font.setPointSize(title_font_pt_size);
    else
	title_font.setPixelSize(title_font_px_size);
    title->setFont(title_font);

    desc = new QLabel(this);
    sp = QSizePolicy( QSizePolicy::Minimum, QSizePolicy::Maximum );
    sp.setHeightForWidth( desc->sizePolicy().hasHeightForWidth() );
    desc->setSizePolicy(sp);
    //QFont desc_font( decs->font() );
    //desc_font.setBold( true );
    //desc->setFont(decs_font);

    separator = new QFrame(this);
    sp = QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    sp.setHeightForWidth( separator->sizePolicy().hasHeightForWidth() );
    separator->setSizePolicy(sp);
    separator->setFrameStyle(QFrame::HLine|QFrame::Sunken);

    modlist = new ACenterSectionModulesList(this);
    modlist->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout *txt_layout = new QVBoxLayout();
    txt_layout->addWidget(title);
    txt_layout->addWidget(desc);
    txt_layout->addWidget(modlist);

    QHBoxLayout *h_layout = new QHBoxLayout();
    h_layout->addWidget(pixmap);
    h_layout->addLayout(txt_layout);

    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->addLayout(h_layout);
    main_layout->addWidget(separator);
}

ACenterSection::~ACenterSection()
{}

void ACenterSection::setPixmap(const QPixmap &pix)
{
    pixmap->setPixmap(pix);
}

void ACenterSection::setText(const QString &txt)
{
    title->setText(txt);
}

void ACenterSection::setDesc(const QString &txt)
{
    desc->setText(txt);
}

ACenterSectionModulesList* ACenterSection::getModulesList()
{
    return modlist;
}

/* ACenterFace */
ACenterFace::ACenterFace(QWidget *parent, const Qt::Orientation o):
    AWidget<QFrame>(parent)
{
    setObjectName("centerface");
    setLineWidth(0);
    setFrameStyle(QFrame::Plain | QFrame::NoFrame);

    reg_events = BrowserEventUnknown;
    current_action_key = "__undefined__";
    current_module_key = "__undefined__";

    owerview_btn = new QToolButton(this);
    owerview_btn->setEnabled(false);
    owerview_btn->setAutoRaise(true);
    owerview_btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    owerview_btn->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed);
    owerview_btn->setText(tr("Contents"));
    owerview_btn->setIcon(getPixmap("theme:left"));
    connect(owerview_btn, SIGNAL(clicked()), this, SLOT(onOwerviewClicked()));

    QToolButton *help_btn = new QToolButton(this);
    help_btn->setAutoRaise(true);
    help_btn->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    help_btn->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Fixed);
    help_btn->setText(tr("Help"));
    help_btn->setIcon(getPixmap("theme:help"));
    connect(help_btn, SIGNAL(clicked()), this, SLOT(onHelpClicked()));

    sections_widget = new QWidget(this);
    module_widget = new QWidget(this);
    buttonbox = new QDialogButtonBox(module_widget);

    QScrollArea *scroll = new QScrollArea(module_widget);
    scroll->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    scroll->setWidgetResizable( true );
    QPalette scroll_palette = scroll->palette();
    scroll_palette.setBrush(QPalette::Window, QBrush(QColor(0,0,0,0)));
    scroll->setPalette(scroll_palette);

    view_widget = new QWidget();
    view_widget->setObjectName("view");
    QPalette vw_palette = view_widget->palette();
    vw_palette.setBrush(QPalette::Window, QBrush(QColor(0,0,0,0)));
    view_widget->setPalette(vw_palette);

    scroll->setWidget(view_widget);

    QScrollArea *sections_scroll = new QScrollArea(sections_widget);
    sections_scroll->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    sections_scroll->setWidgetResizable( true );
    QPalette ss_palette = sections_scroll->palette();
    ss_palette.setBrush(QPalette::Window, QBrush(QColor(0,0,0,0)));
    sections_scroll->setPalette(ss_palette);

    sections_view_widget = new QWidget();
    sections_view_widget->setObjectName("modules_view");
    QPalette svw_palette = sections_view_widget->palette();
    svw_palette.setBrush(QPalette::Window, QBrush(QColor(0,0,0,0)));
    sections_view_widget->setPalette(svw_palette);

    sections_scroll->setWidget(sections_view_widget);

    Qt::Orientation orient = Utils::fixOrientation(o, Qt::Vertical);
    if( orient == Qt::Horizontal )
	view_layout = new QHBoxLayout(view_widget);
    else
	view_layout = new QVBoxLayout(view_widget);
    QVBoxLayout *sections_view_main_layout = new QVBoxLayout(sections_view_widget);
    sections_view_layout = new QVBoxLayout();
    sections_view_main_layout->addLayout(sections_view_layout);
    sections_view_main_layout->addStretch(1);

    QVBoxLayout *sections_layout = new QVBoxLayout(sections_widget);
    sections_layout->addWidget(sections_scroll);
    sections_layout->setMargin(0);
    sections_layout->setSpacing(0);
    module_layout = new QVBoxLayout(module_widget);
    module_layout->setMargin(0);
    module_layout->setSpacing(0);
    module_layout->addWidget(scroll);
    module_layout->addWidget(buttonbox);

    QHBoxLayout *top_buttons_layout = new QHBoxLayout();
    top_buttons_layout->addWidget(owerview_btn);
    top_buttons_layout->addWidget(help_btn);
    top_buttons_layout->addStretch(1);

    stacked_layout = new QStackedLayout();
    stacked_layout->setMargin(0);
    stacked_layout->setSpacing(0);
    stacked_layout->addWidget(sections_widget);
    stacked_layout->addWidget(module_widget);
    stacked_layout->setCurrentWidget(sections_widget);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->setMargin(0);
    main_layout->setSpacing(0);
    main_layout->addLayout(top_buttons_layout);
    main_layout->addLayout(stacked_layout);
    setLayout(main_layout);

    action_signal_mapper = new QSignalMapper(this);
    connect(action_signal_mapper, SIGNAL(mapped(const QString &)),
	this, SIGNAL(actionSelected(const QString &)));
    connect( this, SIGNAL(actionSelected(const QString&)),
	this, SLOT(onSelectAction(const QString&)) );
}

ACenterFace::~ACenterFace()
{}

void ACenterFace::setEventRegistered(const QString &id_, BrowserEventType e)
{
    id = id_;
    reg_events |= e;
}

bool ACenterFace::eventRegistered(BrowserEventType e)
{
    return reg_events & e;
}

void ACenterFace::onOwerviewClicked()
{
    stacked_layout->setCurrentWidget(sections_widget);
    owerview_btn->setEnabled(false);
}

void ACenterFace::onHelpClicked()
{
    QHelpEvent *hlp = new QHelpEvent((QEvent::Type)EVENT_HELP, QPoint(), QPoint());
    QApplication::postEvent(browser, hlp);
}

void ACenterFace::clearActions()
{
    buttons.clear();
    buttonbox->clear();
}

QPixmap ACenterFace::defaultActionIcon(UserActionType type)
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

void ACenterFace::addAction(const QString& key, const QString& name, const QString& pixmap)
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

void ACenterFace::addAction(const QString &key, UserActionType type)
{
    if( buttons.contains(key) )
	return;

    QPushButton *b = buttonbox->addButton("", QDialogButtonBox::ActionRole);
    b->setIcon(QIcon(defaultActionIcon(type)));
    buttons[key] = b;
    connect(b, SIGNAL(clicked()), action_signal_mapper, SLOT(map()));
    action_signal_mapper->setMapping(b, key);
}

void ACenterFace::removeAction(const QString &key)
{
    QAbstractButton *b = buttons.take(key);
    if(b)
    {
	buttonbox->removeButton(b);
    }
}

void ACenterFace::setActionText(const QString &key, const QString &value)
{
    if( buttons.contains(key) )
	buttons[key]->setText(value);
}

void ACenterFace::setActionPixmap(const QString &key, const QString &value)
{
    if( buttons.contains(key) )
	buttons[key]->setIcon(getPixmap(value));
}

void ACenterFace::clearModules()
{
    QList<ACenterSectionModulesListItem*> dead_modules = modules.values();
    modules.clear();
    QListIterator<ACenterSectionModulesListItem*> it(dead_modules);
    while(it.hasNext())
    {
	ACenterSectionModulesListItem *dead = it.next();
        delete dead;
        dead = 0;
    }
}

void ACenterFace::addModule(const QString& section_key, const QString& key, const QString& name)
{
    if( !key.isEmpty() && sections.contains(section_key) )
    {
	ACenterSection *section = sections[section_key];
	ACenterSectionModulesList *mlist = section->getModulesList();
	ACenterSectionModulesListItem *i = new ACenterSectionModulesListItem();
	mlist->addItem(i);
	modules[key] = i;
	setModuleText(key,name);
    }
}

void ACenterFace::setModuleText(const QString &key, const QString &value)
{
    if( modules.contains(key) )
    {
	modules[key]->setText(value);
    }
}

void ACenterFace::setModulePixmap(const QString &key, const QString &value)
{
#if 0
    if( modules.contains(key) )
	modules[key]->setIcon(getPixmap(value));
#endif
}

void ACenterFace::removeModule(const QString &key)
{
    if( modules.contains(key) )
    {
	ACenterSectionModulesListItem* i = modules[key];
	QMapIterator<QString,ACenterSection*> it(sections);
	while(it.hasNext())
	{
	    ACenterSection *sect = it.next().value();
	    ACenterSectionModulesList *mlst = sect->getModulesList();
	    if( mlst->isOwnerOfItem(i) )
	    {
		modules.remove(key);
		mlst->removeItem(i);
	    }
	}
    }
}

void ACenterFace::clearSections()
{
    clearModules();
    QMap<QString,ACenterSection*> dead_sections = sections;
    sections.clear();

    QMapIterator<QString,ACenterSection*> it(dead_sections);
    while(it.hasNext())
    {
	delete it.next().value();
	dead_sections[it.key()] = 0;
    }
}

void ACenterFace::addSection(const QString& key, const QString& name, const QString& desc, const QString& pixmap)
{
    ACenterSection *section = new ACenterSection(sections_view_widget, name);
    sections_view_layout->addWidget(section);
    sections[key] = section;
    if(!desc.isEmpty())
	setSectionDesc(key, desc);
    if(!pixmap.isEmpty())
	setSectionPixmap(key, pixmap);
    connect(section->getModulesList(), SIGNAL(itemClicked(ACenterSectionModulesListItem*)), this, SLOT(onSelectModule(ACenterSectionModulesListItem*)));
}

void ACenterFace::setSectionText(const QString &key, const QString &value)
{
    if( sections.contains(key) )
	sections[key]->setText(value);
}

void ACenterFace::setSectionDesc(const QString &key, const QString &value)
{
    if( sections.contains(key) )
	sections[key]->setDesc(value);
}

void ACenterFace::setSectionPixmap(const QString &key, const QString &value)
{
    if( sections.contains(key) )
	sections[key]->setPixmap(getPixmap(value));
}

void ACenterFace::removeSection(const QString& key)
{
    if( sections.contains(key) )
    {
	ACenterSection *section = sections.take(key);
	ACenterSectionModulesList *mlistw = section->getModulesList();
	QList<QString> mod_keys;
	QMapIterator<QString,ACenterSectionModulesListItem*> it(modules);
	while(it.hasNext())
	{
	    if( mlistw->isOwnerOfItem(it.next().value()) )
		mod_keys.append(it.key());
	}
	QListIterator<QString> mit(mod_keys);
	while(mit.hasNext())
	    removeModule(mit.next());
	delete section;
    }
}

QWidget* ACenterFace::getViewWidget()
{
    return view_widget;
}

QLayout* ACenterFace::getViewLayout()
{
    return view_layout;
}

void ACenterFace::onSelectAction(const QString& key)
{
    UserActionType type = enums->strToUserAction(key);
    if( type == UserActionHelp )
    {
	QHelpEvent *hlp = new QHelpEvent((QEvent::Type)EVENT_HELP, QPoint(), QPoint());
	QApplication::postEvent(browser, hlp);
    }
    current_action_key = key;
    if( eventRegistered(BrowserEventClicked) )
	browser->emitEvent(id, BrowserEventClicked, AlteratorRequestDefault);
}

void ACenterFace::onSelectModule(ACenterSectionModulesListItem *i)
{
    current_module_key = modules.key(i);
    view_widget->hide();
    if( eventRegistered(BrowserEventSelected) )
	browser->emitEvent(id, BrowserEventSelected, AlteratorRequestCenterFaceModuleSelected);
    stacked_layout->setCurrentWidget(module_widget);
    owerview_btn->setEnabled(true);
}

QString ACenterFace::currentActionKey()
{
    return current_action_key;
}

QString ACenterFace::currentModuleKey()
{
    return current_module_key;
}

/* alCenterFace */
alCenterFace::alCenterFace(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
    alWidgetPre<ACenterFace>(attr,WCenterFace,id,parent)
{
}

alCenterFace::~alCenterFace(){}

QWidget* alCenterFace::getViewWidget()
{
    return wnd_->getViewWidget();
}

QLayout* alCenterFace::getViewLayout()
{
    return wnd_->getViewLayout();
}

void alCenterFace::registerEvent(const QString& name)
{
    if ("clicked" == name)
	wnd_->setEventRegistered(id_, BrowserEventClicked);
    if ("selected" == name)
	wnd_->setEventRegistered(id_, BrowserEventSelected);
}

QString alCenterFace::postData() const
{
    QString ret;
    QString current_action = wnd_->currentActionKey();
    if(!current_action.isEmpty())
	ret += QString(" (current-action . %1)").arg(current_action);
    QString current_module = wnd_->currentModuleKey();
    ret += QString(" (current-module . %1)").arg(current_module);
    return ret;
}

void alCenterFace::setAttr(const QString& name,const QString& value)
{
    if( "actions" == name )
    {
	wnd_->clearActions();
	QStringList data = value.split(";", QString::KeepEmptyParts);
	QStringListIterator it(data);
	forever
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
	QStringList data = value.split(";", QString::KeepEmptyParts);
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
    else if( "action-text" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	const int len = data.size();
	if( len >= 2 )
	    wnd_->setActionText(data[0], data[1]);
    }
    else if( "action-pixmap" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	const int len = data.size();
	if( len >= 2 )
	    wnd_->setActionPixmap(data[0], data[1]);
    }
    else if( "modules" == name )
    {
	wnd_->clearModules();
	QStringList data = value.split(";", QString::KeepEmptyParts);
	QStringListIterator it(data);
	forever
	{
	    QString section_key, key, name;
	    if(it.hasNext())
		section_key = it.next();
	    else
		break;
	    if(it.hasNext())
		key = it.next();
	    else
		break;
	    if(it.hasNext())
		name = it.next();
	    else
		break;
	    wnd_->addModule(section_key, key, name);
	}

    }
    else if( "module-add" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	if( data.size() >= 3 )
	    wnd_->addModule(data[0], data[1], data[2]);
    }
    else if( "module-remove" == name )
    {
	wnd_->removeModule(value);
    }
    else if( "modules-clear" == name )
    {
	wnd_->clearModules();
    }
    else if( "module-text" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	const int len = data.size();
	if( len >= 2 )
	    wnd_->setModuleText(data[0], data[1]);
    }
    else if( "module-pixmap" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	const int len = data.size();
	if( len >= 2 )
	    wnd_->setModulePixmap(data[0], data[1]);
    }
    else if( "sections" == name )
    {
	wnd_->clearSections();
	QStringList data = value.split(";", QString::KeepEmptyParts);
	QStringListIterator it(data);
	forever
	{
	    QString key, name, desc, pixmap;
	    if(it.hasNext())
		key = it.next();
	    else
		break;
	    if(it.hasNext())
		name = it.next();
	    else
		break;
	    if(it.hasNext())
		desc = it.next();
	    else
		break;
	    if(it.hasNext())
		pixmap = it.next();
	    else
		break;
	    wnd_->addSection(key, name, desc, pixmap);
	}
    }
    else if( "sections-clear" == name )
    {
	wnd_->clearSections();
    }
    else if( "section-add" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	if( data.size() >= 4 )
	    wnd_->addSection(data[0], data[1], data[2], data[3]);
    }
    else if( "section-remove" == name )
    {
	wnd_->removeSection(value);
    }
    else if( "section-text" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	const int len = data.size();
	if( len >= 2 )
	    wnd_->setSectionText(data[0], data[1]);
    }
    else if( "section-pixmap" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	const int len = data.size();
	if( len >= 2 )
	    wnd_->setSectionPixmap(data[0], data[1]);
    }
    else
	alWidget::setAttr(name,value);
}
