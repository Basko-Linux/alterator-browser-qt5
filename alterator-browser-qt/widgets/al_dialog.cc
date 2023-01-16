#include <QScrollBar>

#include "al_dialog.hh"

ADialog::ADialog(QWidget *parent, const Qt::Orientation orient):
    AWidget<ADialogBase>(parent)
{
    current_action = QStringLiteral("__undefined__");

    key2btn["ok"]=QDialogButtonBox::Ok;
    key2btn["open"]=QDialogButtonBox::Open;
    key2btn["save"]=QDialogButtonBox::Save;
    key2btn["cancel"]=QDialogButtonBox::Cancel;
    key2btn["close"]=QDialogButtonBox::Close;
    key2btn["discard"]=QDialogButtonBox::Discard;
    key2btn["apply"]=QDialogButtonBox::Apply;
    key2btn["reset"]=QDialogButtonBox::Reset;
    key2btn["restoredefaults"]=QDialogButtonBox::RestoreDefaults;
    key2btn["help"]=QDialogButtonBox::Help;
    key2btn["saveall"]=QDialogButtonBox::SaveAll;
    key2btn["yes"]=QDialogButtonBox::Yes;
    key2btn["yestoall"]=QDialogButtonBox::YesToAll;
    key2btn["no"]=QDialogButtonBox::No;
    key2btn["notoall"]=QDialogButtonBox::NoToAll;
    key2btn["abort"]=QDialogButtonBox::Abort;
    key2btn["retry"]=QDialogButtonBox::Retry;
    key2btn["ignore"]=QDialogButtonBox::Ignore;
    key2btn["nobutton"]=QDialogButtonBox::NoButton;
    //qDebug("new ADialog");
//    setWindowModality(Qt::ApplicationModal);
    QGridLayout *main_layout = new QGridLayout(view());
    if( g_browser->haveWindowManager() )
	main_layout->setMargin(1);
    else
	main_layout->setMargin(10);

    scroll = new QScrollArea(this);
    scroll->setFocusPolicy(Qt::NoFocus);
    { // install event filter for scroll area
	QScrollBar *vs = scroll->verticalScrollBar();
	if( vs )
	    vs->installEventFilter(this);
	QScrollBar *hs = scroll->horizontalScrollBar();
	if( hs )
	    hs->installEventFilter(this);
    }
    scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scroll->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    scroll->setWidgetResizable( true );

    view_widget = new QWidget(this);
//    view_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    scroll->setWidget(view_widget);

    Qt::Orientation o = Utils::fixOrientation(orient, Qt::Vertical);
    QBoxLayout *view_layout;
    if( o == Qt::Horizontal )
	view_layout = new QHBoxLayout(view_widget);
    else
	view_layout = new QVBoxLayout(view_widget);

    view_layout->setSpacing(5);
    view_layout->setMargin(5);

    btnbox = new QDialogButtonBox(this);

    main_layout->addWidget(scroll, 0, 0);
    main_layout->addWidget(btnbox, 1, 0);

    connect(btnbox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
    connect(this, SIGNAL(finished(int)), this, SLOT(onFinish(int)));
}

ADialog::~ADialog()
{
}

void ADialog::onFinish(int)
{
    g_browser->popupRemove(this);
}

QWidget* ADialog::getView()
{
    return view_widget;
}

bool ADialog::eventFilter(QObject *o, QEvent *e)
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

    return ADialogBase::eventFilter(o, e);
}

#if 0
void ADialog::showEvent(QShowEvent *e)
{
    //qDebug("ADialog::showEvent");
    if( !g_browser->haveWindowManager() )
    {
	Utils::fixWmlessPopup(this);
    }
    e->accept();
}

void ADialog::paintEvent(QPaintEvent* e)
{
    //QDialog::paintEvent(e);
    if( !g_browser->haveWindowManager() )
    {
	Utils::widgetCornersRound(this);
    }
}

void ADialog::closeEvent(QCloseEvent *e)
{
    e->ignore();
}

void ADialog::keyPressEvent ( QKeyEvent * e ) 
{
    switch( e->key() )
    {
	case Qt::Key_Escape:
	case Qt::Key_Enter:
	case Qt::Key_Return:
	{
	    e->ignore();
	    break;
	}
	default:
	    ADialogBase::keyPressEvent(e);
    }
}
#endif

void ADialog::addAction(const QString& key, const QString& name, const QString& pixmap)
{
    if( !key.isEmpty() && !buttons.contains(key) )
    {
	QPushButton *b = 0;
	if( key2btn.contains(key) )
	    b = btnbox->addButton(key2btn[key]);
        else
	    b = btnbox->addButton(name, QDialogButtonBox::ActionRole);
	if( b )
	    buttons[key] = b;
    }
}

void ADialog::removeAction(const QString &key)
{
    if( buttons.contains(key) )
    {
	QAbstractButton *b = buttons.take(key);
	if( b )
	    btnbox->removeButton(b);
    }
}

void ADialog::clearActions()
{
    buttons.clear();
    btnbox->clear();
}

void ADialog::onButtonClicked(QAbstractButton* btn)
{
    if( btn )
    {
	QMapIterator<QString, QAbstractButton*> it(buttons);
	while(it.hasNext() )
	{
	    if( btn == it.next().value() )
	    {
		current_action = it.key();
		Q_EMIT actionSelected();
	    }
	}
    }
}

void ADialog::setDefaultAction(const QString& key)
{
    if( buttons.contains(key) )
    {
	QAbstractButton *btn = buttons[key];
	QPushButton *button = qobject_cast<QPushButton*>(btn);
	if( button )
	    button->setDefault(true);
    }
}

QString ADialog::currentAction()
{
    return current_action;
}

// alDialog
alDialog::alDialog(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
    alWidgetPre<ADialog>(attr,WDialog,id,parent)
{
}

void alDialog::popUp()
{
    QTimer::singleShot(0, wnd_, SLOT(exec()));
}

void alDialog::popDown()
{
    wnd_->done(0);
}

QWidget* alDialog::getViewWidget()
{
    return wnd_->getView();
}

QLayout* alDialog::getViewLayout()
{
    return wnd_->getView()->layout();
}

void alDialog::registerEvent(const QString& name)
{
    if ("clicked" == name)
    {
	connect(wnd_,SIGNAL(actionSelected()), SLOT(onClick()));
    }
}

void alDialog::setAttr(const QString& name,const QString& value)
{
    if( "actions" == name )
    {
	wnd_->clearActions();
	QStringList data = value.split(QLatin1String(";"), Qt::KeepEmptyParts);
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
	QStringList data = value.split(QLatin1String(";"), Qt::KeepEmptyParts);
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
    else if( "action-default" == name )
    {
	wnd_->setDefaultAction(value);
    }
    else if ("width" == name)
    {
	bool ok;
	int wdth = value.toInt(&ok);
	if( ok && wdth > 0 )
	    wnd_->setFixedWidth(wdth);
    }
    else if ("height" == name)
    {
	bool ok;
	int h = value.toInt(&ok);
	if( ok && h > 0 )
	    wnd_->setFixedHeight(h);
    }
    else
	alWidget::setAttr(name,value);
}

QString alDialog::postData() const
{
    QString ret;
    QString current_action = wnd_->currentAction();
    if(!current_action.isEmpty())
	ret += QString(" (current-action . %1)").arg(current_action);
    return ret;
}
