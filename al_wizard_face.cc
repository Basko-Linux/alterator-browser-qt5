
#include "global.hh"
#include "widgets.hh"
#include "al_wizard_face.hh"

extern alWizardFace *wizard_face;

// AWizardFace
AWizardFace::AWizardFace( QWidget *parent, Qt::WFlags f):
    QWidget(parent, f)
{
    main_layout = new QGridLayout(this);
    labels_layout = new QVBoxLayout();
    main_layout->addLayout( labels_layout, 0, 0 );

    view = new QFrame(this);
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    view->show();
    main_layout->addWidget(view, 0, 1);

    buttons_layout = new QHBoxLayout();
    main_layout->addLayout( buttons_layout, 0, 0, 1, -1);
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
	    QPushButton *b = new QPushButton(this);
	    buttons_layout->addWidget(b);
	    w = buttons[id] = b;
	    break;
	case LabelGeneric:
	case LabelSection:
	    QLabel *l = new QLabel(this);
	    labels_layout->addWidget(l);
	    w = labels[id] = l;
	    break;
	default:
	    break;
    }
    return w;
}

QWidget* AWizardFace::getView()
{
    return view;
}

QWidget* AWizardFace::getItemWidget(const QString &id)
{
    if( buttons.contains(id) )
	return buttons[id];
    else if( labels.contains(id) )
	return labels[id];
    else
	return 0;
}

void AWizardFace::setItemText(const QString &id, const QString &value)
{
    if( buttons.contains(id) )
	buttons[id]->setText(value);
    else if( labels.contains(id) )
	labels[id]->setText(value);
}

void AWizardFace::setItemPixmap(const QString &id, const QString &value)
{
    if( buttons.contains(id) )
	buttons[id]->setIcon(QIcon(value));
    else if( labels.contains(id) )
	labels[id]->setPixmap(value);
}


// alWizardFaceItem
alWizardFaceItem::alWizardFaceItem(const QString& id,const QString& parent, QWidget* wnd):
    alWidget(id, parent)
{
    wnd_ = wnd;
}

alWizardFaceItem::~alWizardFaceItem(){}

void alWizardFaceItem::setAttr(const QString& name,const QString& value)
{
    if( wizard_face )
    {
	if ("text" == name)
	    wizard_face->getWidget()->setItemText(getId(), value);
	else if ("pixmap" == name)
	    wizard_face->getWidget()->setItemPixmap(getId(), IMAGES_PATH+value);
	else
	    alWidget::setAttr(name,value);
    }
}

void alWizardFaceItem::registerEvent(const QString& name)
{
    if ("on-click" == name)
    {
	if( wizard_face )
	{
	    QWidget *widget = wizard_face->getWidget()->getItemWidget(getId());
	    if( widget )
		connect( widget, SIGNAL(clicked()), SLOT(onClick()) );
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
