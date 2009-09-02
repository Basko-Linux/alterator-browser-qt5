
#include "browser.hh"

#include "al_widget.hh"
#include "utils.hh"
#include "widgetlist.hh"

alWidget::alWidget(AlteratorWidgetType type, const QString& id,const QString& parent):
    QObject(widgetlist->qWidgetById(parent)),
    type_(type),
    id_(id),
    parent_(parent),
    children_alignment(Qt::AlignJustify)
{
    o_wnd_ = 0;
    wnd_destroyed = true;
    widgetlist->add(id, this);
}

alWidget::~alWidget()
{
    widgetlist->removeFromListById(getId());
    if( o_wnd_ && !wnd_destroyed)
    {
	widgetlist->groupRemove(this);
	o_wnd_->deleteLater();
	o_wnd_ = 0;
    }
}

void alWidget::onUpdate() { emit updated(); }
void alWidget::onUpdate(int) { emit updated(); }

void alWidget::onClick()     { browser->emitEvent(id_, "clicked", AlteratorRequestDefault); }
void alWidget::onSpecialClick(const AlteratorRequestFlags flags)     { browser->emitEvent(id_,"clicked", flags); }
void alWidget::onClick(bool) { browser->emitEvent(id_,"clicked", AlteratorRequestDefault); }
void alWidget::onClick(QListWidgetItem*)     { browser->emitEvent(id_,"clicked", AlteratorRequestDefault); }
void alWidget::onClick(QTreeWidgetItem*,int) { browser->emitEvent(id_,"clicked", AlteratorRequestDefault); }

void alWidget::onChange()    { browser->emitEvent(id_,"changed", AlteratorRequestDefault); }
void alWidget::onChange(int) { browser->emitEvent(id_,"changed", AlteratorRequestDefault); }
void alWidget::onChange(const QString&) { browser->emitEvent(id_,"changed", AlteratorRequestDefault); }
void alWidget::onChange(QWidget*)       { browser->emitEvent(id_,"changed", AlteratorRequestDefault); }

void alWidget::onReturn() { browser->emitEvent(id_,"return-pressed", AlteratorRequestDefault); }
void alWidget::onReturn(QListWidgetItem*) { browser->emitEvent(id_,"return-pressed", AlteratorRequestDefault); }
void alWidget::onReturn(QTreeWidgetItem*, int) { browser->emitEvent(id_,"return-pressed", AlteratorRequestDefault); }

void alWidget::onSelect() { browser->emitEvent(id_,"selected", AlteratorRequestDefault); }
void alWidget::onSelect(int n) { browser->emitEvent(id_,"selected", AlteratorRequestDefault); }
void alWidget::onSelect(QTreeWidgetItem*,QTreeWidgetItem*) { browser->emitEvent(id_,"selected", AlteratorRequestDefault); }
void alWidget::onSelect(const QString& s) { browser->emitEvent(id_,"selected", AlteratorRequestDefault); }

void alWidget::onDoubleClick() { browser->emitEvent(id_,"double-clicked", AlteratorRequestDefault); }
void alWidget::onDoubleClick(QTreeWidgetItem*,int) { browser->emitEvent(id_,"double-clicked", AlteratorRequestDefault); }
void alWidget::onDoubleClick(QListWidgetItem*) { browser->emitEvent(id_,"double-clicked", AlteratorRequestDefault); }

void alWidget::onToggle(bool) { browser->emitEvent(id_,"toggled", AlteratorRequestDefault); }
void alWidget::onToggle(int) { browser->emitEvent(id_,"toggled", AlteratorRequestDefault); }

void alWidget::onEvent(const BrowserEventType e, const AlteratorRequestFlags f)
{
    browser->emitEvent(id_, e, f);
}

void alWidget::onEvent(const BrowserEventType e)
{
    onEvent(e, AlteratorRequestDefault);
}

void alWidget::setAttr(const QString& name,const QString& value)
{
	QWidget *w = getWidget();
	QLayout *l = getViewLayout();

	if ("visibility" == name)
	{
	    if(w)
		w->setVisible("true" == value);
	}
	else if ("activity" == name)
	{
	    if(w)
		w->setEnabled("true" == value);
	}
	else if ("widget-name" == name)
		setObjectName(value);
	else if ("name" == name)
	{
	    if(w)
	    {
		group_ = value;
		w->setProperty("altgroup", value);
		widgetlist->groupAdd(this);
	    }
	}
	else if ("tooltip" == name)
	{
	    QStringList args = value.split(";");
	    QString tip = args[0];
	    QString sep = ".<br>";
	    int sep_len = sep.size();
	    if( !tip.isEmpty() && !tip.contains("<br") )
	    {
		int pos = 0;
		do
		{
		    pos = tip.indexOf(QRegExp("\\.\\s"), pos+1);
		    if( pos >= 0 )
		    {
			tip.replace(pos, 2, sep);
			pos += sep_len;
		    }
		    else
			break;
		}
		while(pos < tip.size());
	    }
	    if(w)
		w->setToolTip(tip);
	}
	else if ("window-title" == name || "title" == name)
	{
	    if(w)
	    {
		Popup *p = qobject_cast<Popup*>(w);
		if( p )
		    p->setPopupTitle(value);
		else
		    w->window()->setWindowTitle(value);
	    }
	}
	else if ("max-width" == name)
	{
	    if(w)
		w->setMaximumWidth(value.toInt());
	}
	else if ("max-height" == name)
	{
	    if(w)
		w->setMaximumHeight(value.toInt());
	}
	else if ("min-width" == name)
	{
	    if(w)
		w->setMinimumWidth(value.toInt());
	}
	else if ("min-height" == name)
	{
	    if(w)
		w->setMinimumHeight(value.toInt());
	}
	else if ("width" == name)
	{
	    if(w)
		w->setFixedWidth(value.toInt());
	}
	else if ("height" == name)
	{
	    if(w)
		w->setFixedHeight(value.toInt());
	}
	else if ("margin" == name)
	{
	    if( l )
		l->setMargin(value.toInt());
	}
	else if ("spacing" == name)
	{
	    if( l )
		l->setSpacing(value.toInt());
	}
	else if("clear-layout" == name)
	{
	    widgetlist->deleteChildrenById(getId());
	}
	else if ("align" == name)
	{
	    alWidget *aw = widgetlist->alWidgetById(getParentId());
	    if( aw )
	    {
		QLayout *l = aw->getViewLayout();
		if( l )
		    l->setAlignment(w, Utils::convertAlign(value));
	    }
	}
	else if ("help" == name)
	{
	    browser->setHelpSource(value);
	}
	else if ("focus" == name)
	{
	    if(w)
	    {
		if( "true" == value )
		    w->setFocus();
		else
		    w->clearFocus();
	    }
	}
	else if ("tab-order" == name)
	{
	    QWidget *first = 0;
	    alWidget *aw = widgetlist->alWidgetById(value);
	    if( aw )
		first = aw->getWidget();
	    if( w && first )
	    {
		if( first->focusProxy() )
		    first = first->focusProxy();
		if( w->focusProxy() )
		    w = w->focusProxy();
		w->setTabOrder(first, w);
	    }
	}
}

QSizePolicy alWidget::adjustSizePolicy(const AlteratorWidgetType type, const QSizePolicy policy, const Qt::Orientation orient, const Qt::Orientation)
{
    QSizePolicy szpol = policy;
    switch( type )
    {
	case WBox:
	case WVBox:
	case WHBox:
	case WTabBox:
	case WVTabBox:
	case WHTabBox:
	case WGroupBox:
	case WVGroupBox:
	case WHGroupBox:
	{
	    if( orient == Qt::Horizontal )
		szpol.setHorizontalPolicy(QSizePolicy::Expanding);
	    else
		szpol.setVerticalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case WGridBox:
	case WTextBox:
	case WSplitBox:
	{
	    szpol.setVerticalPolicy(QSizePolicy::Expanding);
	    szpol.setHorizontalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case WTree:
	case WListBox:
	case WRadioListBox:
	case WMultiListBox:
	case WCheckListBox:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Preferred);
	    szpol.setVerticalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case WComboBox:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Preferred);
	    break;
	}
	case WProgressBar:
	case WSlider:
	case WCheckBox:
	case WRadio:
	{
	    //szpol.setVerticalPolicy(QSizePolicy::Maximum);
	    //szpol.setHorizontalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case WLabel:
	{
	    szpol.setVerticalPolicy(QSizePolicy::Fixed);
	    break;
	}
	case WEdit:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case WButton:
	case WLinkButton:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Maximum);
	    break;
	}
	case WSeparator:
	{
/*
	    if( parent_orient == Qt::Horizontal )
	    {
		szpol.setHorizontalPolicy(QSizePolicy::Fixed);
		szpol.setVerticalPolicy(QSizePolicy::Expanding);
	    }
	    else
	    {
		szpol.setHorizontalPolicy(QSizePolicy::Expanding);
		szpol.setVerticalPolicy(QSizePolicy::Fixed);
	    }
*/
	    break;
	}
	case WTabPage:
	case WVTabPage:
	case WHTabPage:
	case WSlideShow:
	case WTimeEdit:
	case WDateEdit:
	case WSpacer:
	case WSpinBox:
	case WProxy:
	case WDialog:
	case WMainWidget:
	case WWizardFace:
	case WCenterFace:
	case WUnknown:
	    break;
/*	default:
	{
	    //qDebug("don't set size policy");
	} */
    }
    return szpol;
}

void alWidget::setChildrenAlignment(Qt::Alignment a)
{
    children_alignment = a;
}

Qt::Alignment alWidget::childrenAlignment()
{
    return children_alignment;
}

void alWidget::postAddChild(QWidget* chld, AlteratorWidgetType type, const AlteratorRequestActionAttrs &attr)
{
    switch( type )
    {
	case WDialog:
	{
	    break;
	}
	default:
	{
	    QLayout *l = getViewLayout();
	    if( l )
	    {
		QBoxLayout *bl = qobject_cast<QBoxLayout*>(l);
		if( bl )
		{
		    Qt::Orientation orientation = Qt::Horizontal;
		    if( bl->direction() == QBoxLayout::TopToBottom || bl->direction() == QBoxLayout::BottomToTop )
			orientation = Qt::Vertical;
		    chld->setSizePolicy(adjustSizePolicy( type, chld->sizePolicy(), attr[AltReqParamWOrientation].o, orientation ));
		    if( childrenAlignment() != Qt::AlignJustify )
			bl->addWidget(chld, 0, childrenAlignment());
		    else
			bl->addWidget(chld);
		}
		else
		    l->addWidget(chld);
	    }
	}
    }
}

void alWidget::setWndObject(QObject *wnd)
{
    if( wnd && !o_wnd_ )
    {
	o_wnd_ = wnd;
	wnd_destroyed = false;
	connect(o_wnd_, SIGNAL(destroyed(QObject*)), this, SLOT(onWndDestroyed(QObject*)));
    }
}

void alWidget::onWndDestroyed(QObject *dead)
{
    if( dead == o_wnd_ ) {
	wnd_destroyed = true;
	widgetlist->groupRemove(this);
	deleteLater();
    }
}

// non-object
alWidget* alWidgetCreateWidgetGetParent(const QString& parent)
{
    return widgetlist->alWidgetById(parent);
}
