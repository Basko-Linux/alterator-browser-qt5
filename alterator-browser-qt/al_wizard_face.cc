
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
    main_layout->addWidget( labels_widget, 0, 0 );
    main_layout->addWidget( scroll, 0, 1);
    main_layout->addWidget( buttons_widget, 1, 0, 1, 2);
}

AWizardFace::~AWizardFace()
{}

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
		buttons_layout->addWidget(b);
		w = buttons[id] = b;
		break;
	    }
	case LabelSection:
	case LabelGeneric:
	    {
		QPushButton *l = new QPushButton(labels_widget);
		l->setFlat(true);
		labels_layout->addWidget(l);
		w = buttons[id] = l;
		break;
	    }
	default:
	    {
		QPushButton *lg = new QPushButton(labels_widget);
		lg->setFlat(true);
		labels_layout->addWidget(lg);
		w = buttons[id] = lg;
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
}


// alWizardFaceItem
alWizardFaceItem::alWizardFaceItem(const QString& id,const QString& parent, QWidget* wnd):
    alWidget(id, parent)
{
    elements[id] = this;
    wnd_ = wnd;
}

alWizardFaceItem::~alWizardFaceItem(){}

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
    alWidgetPre<AWizardFace>(id,parent)
{
    new MyBoxLayout(wnd_->getView(), MyBoxLayout::vertical);
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
    alWidget::setAttr(name,value);
}
