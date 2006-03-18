
#include "widgets.hh"
#include "al_wizard_face.hh"

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

void AWizardFace::addItem(const QString &id, AWizardFace::ItemType, const QString &text)
{
}

QString AWizardFace::lastClickedItem()
{
    qWarning("FIXME! alWizardFace::lastClickedItem");
    return QString("FIXME");
}

QWidget* AWizardFace::viewWidget()
{
    return view;
}


// alWizardFace
alWizardFace::alWizardFace(const QString& id,const QString& parent):
    alWidgetPre<AWizardFace>(id,parent)
{
}

alWizardFace::~alWizardFace(){}

QWidget* alWizardFace::getViewWidget()
{
    return wnd_->viewWidget();
}

QLayout* alWizardFace::getLayout()
{
    return wnd_->viewWidget()->layout();
}

void alWizardFace::registerEvent(const QString& name)
{
    if ("on-click" == name)
    {
	qWarning("FIXME! alWizardFace::registerEvent");
	//connect( wnd_, SIGNAL(itemClicked(QListWidgetItem*)), SLOT(onClick(QListWidgetItem*)) );
    }
}

QString alWizardFace::postData() const
{
    return QString(" (current . ") + wnd_->lastClickedItem() +" )";
}

void alWizardFace::setAttr(const QString& name,const QString& value)
{
    qDebug("alWizardFace::setAttr: <%s> <%s>", name.toLatin1().data(), value.toLatin1().data());
	if ("add-button-generic" == name)
	{
	    qWarning("FIXME!");
	}
	else if( "add-button-help" == name)
	{
	    qWarning("FIXME!");
	}
	else if ("current" == name)
	{
	    qWarning("FIXME!");
	}
	else
		alWidget::setAttr(name,value);
}
