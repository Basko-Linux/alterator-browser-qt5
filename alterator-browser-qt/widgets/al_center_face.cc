
#include <QScrollArea>
#include <QScrollBar>

#include "global.hh"
#include "al_center_face.hh"
#include "a_pixmaps.hh"

extern Enums *enums;

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
    i->setIconSize(QSize(32,32));
    i->setAutoRaise(true);
    i->setBackgroundRole(QPalette::Base);
    i->setForegroundRole(QPalette::Text);
    i->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
    QFontMetrics mtr = QFontMetrics(i->font());
    i->setMinimumWidth(mtr.width("w")*20);
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

/* CenterSection */
ACenterSection::ACenterSection(QWidget *parent, const QString &title_text):
    QWidget(parent)
{
    setObjectName("module_section");

    title = new QLabel(title_text, this);
    QSizePolicy sp( QSizePolicy::Minimum, QSizePolicy::Maximum );
    sp.setHeightForWidth( title->sizePolicy().hasHeightForWidth() );
    title->setSizePolicy(sp);
    QFont title_font( title->font() );
    title_font.setBold( true );
    title->setFont(title_font);

    separator = new QFrame(this);
    sp = QSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    sp.setHeightForWidth( separator->sizePolicy().hasHeightForWidth() );
    separator->setSizePolicy(sp);
    separator->setFrameStyle(QFrame::HLine|QFrame::Sunken);

    modlist = new ACenterSectionModulesList(this);
    modlist->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QVBoxLayout *main_layout = new QVBoxLayout(this);
    main_layout->setMargin(0);
    main_layout->setSpacing(0);
    main_layout->addWidget(title);
    main_layout->addWidget(separator);
    main_layout->addWidget(modlist);
}

ACenterSection::~ACenterSection()
{}

QString ACenterSection::getTitle()
{
    return title->text();
}

ACenterSectionModulesList* ACenterSection::getModulesList()
{
    return modlist;
}

/* ACenterFace */
ACenterFace::ACenterFace(QWidget *parent, const Qt::Orientation o):
    QWidget(parent)
{
    current_action_key = "__undefined__";
    current_module_key = "__undefined__";

    sections_widget = new QWidget(this);
    //sections_widget->setBackgroundRole(QPalette::Base);
    module_widget = new QWidget(this);
    buttonbox = new QDialogButtonBox(module_widget);

    QScrollArea *scroll = new QScrollArea(module_widget);
    //scroll->setBackgroundRole(QPalette::NoRole);
    //scroll->viewport()->setBackgroundRole(QPalette::NoRole);
    scroll->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    scroll->setWidgetResizable( true );

    view_widget = new QWidget(scroll->viewport());
    view_widget->setObjectName("view");
    scroll->setWidget(view_widget);

    QScrollArea *sections_scroll = new QScrollArea(sections_widget);
    sections_scroll->setBackgroundRole(QPalette::Base);
    sections_scroll->viewport()->setBackgroundRole(QPalette::Base);
    sections_scroll->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    sections_scroll->setWidgetResizable( true );

    sections_view_widget = new QWidget(sections_scroll->viewport());
    sections_view_widget->setObjectName("modules_view");
    sections_scroll->setWidget(sections_view_widget);

    Qt::Orientation orient = Utils::fixOrientation(o, Qt::Vertical);
    if( orient == Qt::Horizontal )
	view_layout = new QHBoxLayout(view_widget);
    else
	view_layout = new QVBoxLayout(view_widget);
    sections_view_layout = new QVBoxLayout(sections_view_widget);

    QVBoxLayout *sections_layout = new QVBoxLayout(sections_widget);
    sections_layout->addWidget(sections_scroll);
    module_layout = new QVBoxLayout(module_widget);
    module_layout->addWidget(scroll);
    module_layout->addWidget(buttonbox);

    stacked_layout = new QStackedLayout();
    stacked_layout->setMargin(0);
    stacked_layout->setSpacing(0);
    stacked_layout->addWidget(sections_widget);
    stacked_layout->addWidget(module_widget);
    stacked_layout->setCurrentWidget(sections_widget);

    QVBoxLayout *main_layout = new QVBoxLayout;
    main_layout->setMargin(0);
    main_layout->setSpacing(0);
    main_layout->addLayout(stacked_layout);
    setLayout(main_layout);

    QPushButton *ow_btn = buttonbox->addButton(tr("Owerview"), QDialogButtonBox::ActionRole);
    ow_btn->setIcon(getPixmap("theme:left"));
    connect(ow_btn, SIGNAL(clicked()), this, SLOT(onOwerviewClicked()));

    action_signal_mapper = new QSignalMapper(this);
    connect(action_signal_mapper, SIGNAL(mapped(const QString &)),
	this, SIGNAL(actionSelected(const QString &)));
    connect( this, SIGNAL(actionSelected(const QString&)),
	this, SLOT(onSelectAction(const QString&)) );
}

ACenterFace::~ACenterFace()
{}

void ACenterFace::onOwerviewClicked()
{
    stacked_layout->setCurrentWidget(sections_widget);
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
    modules.clear();
    QList<ACenterSection*> sections = sections_view_widget->findChildren<ACenterSection*>("module_section");
    QListIterator<ACenterSection*> it(sections);
    while(it.hasNext())
    {
        delete it.next();
    }
}

void ACenterFace::addModule(const QString& key, const QString& section_title, const QString& name, const QString& pixmap)
{
    if( !key.isEmpty() && !section_title.isEmpty() )
    {
	ACenterSection *section = 0;
	QList<ACenterSection*> sections = sections_view_widget->findChildren<ACenterSection*>("module_section");
	QListIterator<ACenterSection*> it(sections);
	while(it.hasNext())
	{
	    ACenterSection *sect = it.next();
	    if( sect->getTitle() == section_title )
		section = sect;
	}
	if( !section )
	{
	    section = new ACenterSection(sections_view_widget, section_title);
	    //section->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
	    sections_view_layout->addWidget(section);
	    connect(section->getModulesList(), SIGNAL(itemClicked(ACenterSectionModulesListItem*)), this, SLOT(onSelectModule(ACenterSectionModulesListItem*)));
	}
	ACenterSectionModulesList *mlist = section->getModulesList();
	ACenterSectionModulesListItem *i = new ACenterSectionModulesListItem();
	mlist->addItem(i);
	modules[key] = i;
	setModuleText(key,name);
	setModulePixmap(key,pixmap);
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
    if( modules.contains(key) )
	modules[key]->setIcon(getPixmap(value));
}

void ACenterFace::removeModule(const QString &key)
{
    ACenterSectionModulesListItem* i = modules[key];
    if( i )
    {
	QList<ACenterSection*> sections = sections_view_widget->findChildren<ACenterSection*>("module_section");
	QListIterator<ACenterSection*> it(sections);
	while(it.hasNext())
	{
	    ACenterSection *sect = it.next();
	    ACenterSectionModulesList *mlst = sect->getModulesList();
	    if( mlst == i->parentWidget() )
	    {
		modules.remove(key);
		mlst->removeItem(i);
		if(mlst->count() <= 0)
		    delete sect;
	    }
	}
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
	QApplication::postEvent(main_window, hlp);
    }
    current_action_key = key;
    emit actionSelected();
}

void ACenterFace::onSelectModule(ACenterSectionModulesListItem *i)
{
    current_module_key = modules.key(i);
    stacked_layout->setCurrentWidget(module_widget);
    moduleSelected();
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
    {
	connect(wnd_,SIGNAL(actionSelected()), SLOT(onClick()));
    }
    if ("selected" == name)
	connect(wnd_,SIGNAL(moduleSelected()), SLOT(onSelect()));
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
	    QString key, section, name, pixmap;
	    if(it.hasNext())
		key = it.next();
	    else
		break;
	    if(it.hasNext())
		section = it.next();
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
	    wnd_->addModule(key, section, name, pixmap);
	}

    }
    else if( "module-add" == name )
    {
	QStringList data = value.split(";", QString::KeepEmptyParts);
	if( data.size() >= 4 )
	    wnd_->addModule(data[0], data[1], data[2], data[3]);
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
    else
	alWidget::setAttr(name,value);
}
