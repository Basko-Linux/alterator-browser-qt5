#include <QScrollArea>

#include "al_dialog.hh"

ADialog::ADialog(QWidget *parent):
    QDialog(0)
{
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
    QGridLayout *main_layout = new QGridLayout(this);
    if( main_window->haveWindowManager() )
	main_layout->setMargin(1);
    else
	main_layout->setMargin(10);

    QScrollArea *scroll = new QScrollArea(this);
    scroll->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    scroll->setFrameStyle(QFrame::StyledPanel| QFrame::Sunken);
    scroll->setWidgetResizable( true );

    view_widget = new QWidget(this);
//    view_widget->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    scroll->setWidget(view_widget);

    btnbox = new QDialogButtonBox(this);

    main_layout->addWidget(scroll, 0, 0);
    main_layout->addWidget(btnbox, 1, 0);

    connect(btnbox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonClicked(QAbstractButton*)));
}

QWidget* ADialog::getView()
{
    return view_widget;
}

void ADialog::showEvent(QShowEvent *e)
{
    //qDebug("ADialog::showEvent");
    //QDialog::showEvent(e);
    if( !main_window->haveWindowManager() )
    {
	Utils::fixWmlessPopup(this);
    }
}

void ADialog::paintEvent(QPaintEvent* e)
{
    //QDialog::paintEvent(e);
    if( !main_window->haveWindowManager() )
    {
	Utils::widgetCornersRound(this);
    }
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
	    QDialog::keyPressEvent(e);
    }
}

void ADialog::closeEvent(QCloseEvent *e)
{
    e->ignore();
}

void ADialog::addAction(const QString& key, const QString& name, const QString& pixmap)
{
    if( !key.isEmpty() && !buttons.contains(key) )
    {
	if( key2btn.contains(key) )
	    buttons[key] = btnbox->addButton(key2btn[key]);
        else
	    buttons[key] = btnbox->addButton(name, QDialogButtonBox::ActionRole);
    }
}

void ADialog::removeAction(const QString &key)
{
    if( buttons.contains(key) )
    {
	buttons.remove(key);
	btnbox->removeButton(buttons[key]);
    }
}

void ADialog::clearActions()
{
    buttons.clear();
    btnbox->clear();
}

void ADialog::onButtonClicked(QAbstractButton* btn)
{
    QMapIterator<QString, QAbstractButton*> it(buttons);
    while(it.hasNext() )
    {
	if( btn == it.next().value() )
	    current_action = it.key();
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
alDialog::alDialog(const QString& id,const QString& parent, Qt::Orientation orientation,
    const QString& width,const QString& height):
    alWidgetPre<ADialog>(Dialog,id,parent)
{
    if (!width.isEmpty() || !height.isEmpty())
    {
	int w, h;
	bool w_ok, h_ok;
	w = width.toInt(&w_ok);
	if( !w_ok )
	    w = getWidget()->width();
	h = width.toInt(&h_ok);
	if( !h_ok )
	    h = getWidget()->height();
	getWidget()->resize(w,h);
    }
    QBoxLayout *bl;
    Qt::Orientation o = Utils::fixOrientation(orientation, Qt::Vertical);
    if( o == Qt::Horizontal )
	bl = new QHBoxLayout(getViewWidget());
    else
	bl = new QVBoxLayout(getViewWidget());
    bl->setSpacing(5);
    bl->setMargin(5);
}


QWidget* alDialog::getViewWidget()
{
    return wnd_->getView();
}

QLayout* alDialog::getViewLayout()
{
    return wnd_->getView()->layout();
}

void alDialog::setAttr(const QString& name,const QString& value)
{
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
    else if( "action-default" == name )
    {
	wnd_->setDefaultAction(value);
    }
    else if ("title" == name)
	wnd_->setWindowTitle(value);
    else if ("width" == name)
    {
//	int w = value.toInt();
//	if( w > 0 )
//	    wnd_->resize(w,wnd_->height());
    }
    else if ("height" == name)
    {
//	int h = value.toInt();
//	if( h > 0 )
//	    wnd_->resize(wnd_->width(),h);
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
