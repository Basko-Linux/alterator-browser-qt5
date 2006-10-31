
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
	case ButtonHelp:
	    {
		return 0;
	    }
	case ButtonApply:
	    {
		int pos = findButtonPosition( ButtonCancel );
		if( pos >= 0 )
		    return pos;
		else
		    pos = findButtonPosition( ButtonBackward );
		if( pos >= 0 )
		    return pos;
		else
		    return findButtonPosition( ButtonForward );
	    }
	case ButtonCancel:
	    {
		int pos = findButtonPosition( ButtonApply );
		if( pos >= 0 )
		    return ++pos;
		else
		    pos = findButtonPosition( ButtonBackward );
		if( pos >= 0 )
		    return pos;
		else
		    return findButtonPosition( ButtonForward );
	    }
	case ButtonBackward:
	    {
		int pos = findButtonPosition( ButtonForward );
		if( pos >= 0 )
		    return pos;
		else
		    return -1;
	    }
	case ButtonForward:
	    {
		return -1;
	    }
	case ButtonGeneric:
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
	case ButtonHelp:
	    {
		return Qt::AlignLeft;
	    }
	case ButtonApply:
	case ButtonCancel:
	case ButtonForward:
	case ButtonBackward:
	    {
		return Qt::AlignRight;
	    }
	case ButtonGeneric:
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
	case ButtonHelp:
	    {
		name = "theme:help";
		break;
	    }
	case ButtonApply:
	    {
		name = "theme:apply";
		break;
	    }
	case ButtonCancel:
	    {
		name = "theme:cancel";
		break;
	    }
	case ButtonForward:
	    {
		name = "theme:forward";
		break;
	    }
	case ButtonBackward:
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

QWidget* AWizardFace::addItem(const QString &id, AWizardFace::ItemType type)
{
    QWidget *w = 0;
    
    switch( type )
    {
	case ButtonGeneric:
	case ButtonHelp:
	case ButtonApply:
	case ButtonCancel:
	case ButtonBackward:
	case ButtonForward:
	    {
		QPushButton *b = new QPushButton(buttons_widget);
		setButtonIcon(b, type);
		buttons_layout->insertWidget( newButtonPosition(type), b, 0, newButtonAlignment(type) );
		w = buttons[id] = b;
		types[id] = type;
		break;
	    }
	case LabelSection:
	case LabelGeneric:
	    {
		QPushButton *l = new QPushButton(labels_widget);
		l->setFlat(true);
		labels_layout->addWidget(l);
		w = buttons[id] = l;
		types[id] = type;
		break;
	    }
	default:
	    {
		QPushButton *lg = new QPushButton(labels_widget);
		lg->setFlat(true);
		labels_layout->addWidget(lg);
		w = buttons[id] = lg;
		types[id] = type;
		break;
	    }
    }
    return w;
}

QWidget* AWizardFace::getView()
{
    return view_widget;
}

QWidget* AWizardFace::getItemWidget(const QString &id)
{
    if( buttons.contains(id) )
	return buttons[id];
    else
	return 0;
}

void AWizardFace::setItemText(const QString &id, const QString &value)
{
    //qDebug("%s: AWizardFace::setItemText id<%s> value<%s>", __FUNCTION__, id.toLatin1().data(), value.toLocal8Bit().data());
    if( buttons.contains(id) )
	buttons[id]->setText(value);
}

void AWizardFace::setItemPixmap(const QString &id, const QString &value)
{
    if( buttons.contains(id) )
	buttons[id]->setIcon(QIcon(getPixmap(value)));
}

void AWizardFace::setTitle( const QString &value)
{
    title->setText(value);
}

void AWizardFace::setCurrent( int n )
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

// alWizardFaceItem
alWizardFaceItem::alWizardFaceItem(const QString& id,const QString& parent, QWidget* wnd):
    alWidget(WizardFaceItem, id, parent)
{
//    qDebug("alWizardFaceItem parent=<%s>", parent.toLatin1().data());
    wnd_ = wnd;
}

alWizardFaceItem::~alWizardFaceItem()
{
    wnd_->deleteLater();
}

void alWizardFaceItem::setAttr(const QString& name,const QString& value)
{
//    qDebug("alWizardFaceItem::setAttr: name<%s> value<%s>", name.toLatin1().data(), value.toLatin1().data());
    if( wizard_face )
    {
	// FIXME set pixmap only if wizard_face not custimized
	if ("text" == name)
	    wizard_face->getWidget()->setItemText(getId(), value);
	else if ("pixmap" == name)
	    wizard_face->getWidget()->setItemPixmap(getId(), value);
	else
	    alWidget::setAttr(name,value);
    }
}

void alWizardFaceItem::registerEvent(const QString& name)
{
    if ("clicked" == name)
    {
	if( wizard_face )
	{
	    QWidget *widget = wizard_face->getWidget()->getItemWidget(getId());
	    if( widget )
		connect( widget, SIGNAL(clicked(bool)), SLOT(onClick(bool)) );
	}
    }
}



// alWizardFace
alWizardFace::alWizardFace(const QString& id,const QString& parent):
    alWidgetPre<AWizardFace>(WizardFace,id,parent)
{
    new QVBoxLayout(wnd_->getView());
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
    return "";
}

void alWizardFace::setAttr(const QString& name,const QString& value)
{
    if ("title" == name)
	wnd_->setTitle(value);

    if ("current" == value)
    {
	wnd_->setCurrent( value.toInt() );
    }
    else
	alWidget::setAttr(name,value);
}

void alWizardFace::cleanRequest()
{
    wnd_->cleanRequest();
}
