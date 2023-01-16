
#include "browser.hh"

#include "al_widget.hh"
#include "utils.hh"
#include "widgetlist.hh"

alWidget::alWidget(AlteratorWidgetType type, const QString& id,const QString& parent):
    QObject(g_widgetlist->qWidgetById(parent)),
    type_(type),
    id_(id),
    parent_(parent),
    children_alignment(Qt::AlignJustify)
{
    o_wnd_ = 0;
    wnd_destroyed = true;
    g_widgetlist->add(id, this);
}

alWidget::~alWidget()
{
    g_widgetlist->removeFromListById(getId());
    if( o_wnd_ && !wnd_destroyed)
    {
	g_widgetlist->groupRemove(this);
	o_wnd_->deleteLater();
	o_wnd_ = 0;
    }
}

void alWidget::onUpdate() { Q_EMIT updated(); }
void alWidget::onUpdate(int) { Q_EMIT updated(); }

void alWidget::onClick()     { g_browser->emitEvent(id_, "clicked", AlteratorRequestDefault); }
void alWidget::onSpecialClick(const AlteratorRequestFlags flags)     { g_browser->emitEvent(id_,"clicked", flags); }
void alWidget::onClick(bool) { g_browser->emitEvent(id_,"clicked", AlteratorRequestDefault); }
void alWidget::onClick(QListWidgetItem*)     { g_browser->emitEvent(id_,"clicked", AlteratorRequestDefault); }
void alWidget::onClick(QTreeWidgetItem*,int) { g_browser->emitEvent(id_,"clicked", AlteratorRequestDefault); }

void alWidget::onChange()    { g_browser->emitEvent(id_,"changed", AlteratorRequestDefault); }
void alWidget::onChange(int) { g_browser->emitEvent(id_,"changed", AlteratorRequestDefault); }
void alWidget::onChange(const QString&) { g_browser->emitEvent(id_,"changed", AlteratorRequestDefault); }
void alWidget::onChange(QWidget*)       { g_browser->emitEvent(id_,"changed", AlteratorRequestDefault); }

void alWidget::onReturn() { g_browser->emitEvent(id_,"return-pressed", AlteratorRequestDefault); }
void alWidget::onReturn(QListWidgetItem*) { g_browser->emitEvent(id_,"return-pressed", AlteratorRequestDefault); }
void alWidget::onReturn(QTreeWidgetItem*, int) { g_browser->emitEvent(id_,"return-pressed", AlteratorRequestDefault); }

void alWidget::onSelect() { g_browser->emitEvent(id_,"selected", AlteratorRequestDefault); }
void alWidget::onSelect(int) { g_browser->emitEvent(id_,"selected", AlteratorRequestDefault); }
void alWidget::onSelect(QTreeWidgetItem*,QTreeWidgetItem*) { g_browser->emitEvent(id_,"selected", AlteratorRequestDefault); }
void alWidget::onSelect(const QString&) { g_browser->emitEvent(id_,"selected", AlteratorRequestDefault); }

void alWidget::onDoubleClick() { g_browser->emitEvent(id_,"double-clicked", AlteratorRequestDefault); }
void alWidget::onDoubleClick(QTreeWidgetItem*,int) { g_browser->emitEvent(id_,"double-clicked", AlteratorRequestDefault); }
void alWidget::onDoubleClick(QListWidgetItem*) { g_browser->emitEvent(id_,"double-clicked", AlteratorRequestDefault); }

void alWidget::onToggle(bool) { g_browser->emitEvent(id_,"toggled", AlteratorRequestDefault); }
void alWidget::onToggle(int) { g_browser->emitEvent(id_,"toggled", AlteratorRequestDefault); }

void alWidget::onEvent(const BrowserEventType e, const AlteratorRequestFlags f)
{
    g_browser->emitEvent(id_, e, f);
}

void alWidget::onEvent(const BrowserEventType e)
{
    onEvent(e, AlteratorRequestDefault);
}

void alWidget::setAttr(const QString& name,const QString& value)
{
	QWidget *w = getWidget();
	QLayout *l = getViewLayout();

	if (QStringLiteral("visibility") == name)
	{
	    if(w)
		w->setVisible(QStringLiteral("true") == value);
	}
	else if (QStringLiteral("activity") == name)
	{
	    if(w)
		w->setEnabled(QStringLiteral("true") == value);
	}
	else if (QStringLiteral("widget-name") == name)
		setObjectName(value);
	else if (QStringLiteral("name") == name)
	{
	    if(w)
	    {
		group_ = value;
		w->setProperty("altgroup", value);
		g_widgetlist->groupAdd(this);
	    }
	}
	else if (QStringLiteral("tooltip") == name)
	{
	    QStringList args = value.split(QLatin1String(";"));
	    QString tip = args[0];
	    QString sep = QStringLiteral(".<br>");
	    int sep_len = sep.size();
	    if( !tip.isEmpty() && !tip.contains(QStringLiteral("<br")) )
	    {
		int pos = 0;
		do
		{
		    pos = tip.indexOf(QRegExp(QStringLiteral("\\.\\s")), pos+1);
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
	else if (QStringLiteral("window-title") == name || QStringLiteral("title") == name)
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
	else if (QStringLiteral("max-width") == name)
	{
	    if(w)
	    {
		bool ok;
		int val = value.toInt(&ok);
		if( ok )
		    w->setMaximumWidth(val);
	    }
	}
	else if (QStringLiteral("max-height") == name)
	{
	    if(w)
	    {
		bool ok;
		int val = value.toInt(&ok);
		if( ok )
		    w->setMaximumHeight(val);
	    }
	}
	else if (QStringLiteral("min-width") == name)
	{
	    if(w)
	    {
		bool ok;
		int val = value.toInt(&ok);
		if( ok )
		    w->setMinimumWidth(val);
	    }
	}
	else if (QStringLiteral("min-height") == name)
	{
	    if(w)
	    {
		bool ok;
		int val = value.toInt(&ok);
		if( ok )
		    w->setMinimumHeight(val);
	    }
	}
	else if (QStringLiteral("width") == name)
	{
	    if(w)
	    {
		bool ok;
		int val = value.toInt(&ok);
		if( ok )
		    w->setFixedWidth(val);
	    }
	}
	else if (QStringLiteral("height") == name)
	{
	    if(w)
	    {
		bool ok;
		int val = value.toInt(&ok);
		if( ok )
		    w->setFixedHeight(val);
	    }
	}
	else if (QStringLiteral("margin") == name)
	{
	    if( l )
	    {
		bool ok;
		int val = value.toInt(&ok);
		if( ok )
		    l->setMargin(val);
	    }
	}
	else if (QStringLiteral("spacing") == name)
	{
	    if( l )
	    {
		bool ok;
		int val = value.toInt(&ok);
		if( ok )
		    l->setSpacing(val);
	    }
	}
	else if(QStringLiteral("clear-layout") == name)
	{
	    g_widgetlist->deleteChildrenById(getId());
	}
	else if (QStringLiteral("align") == name)
	{
	    alWidget *aw = g_widgetlist->alWidgetById(getParentId());
	    if( aw )
	    {
		QLayout *l = aw->getViewLayout();
		if( l )
		    l->setAlignment(w, Utils::convertAlign(value));
	    }
	}
	else if (QStringLiteral("help") == name)
	{
	    g_browser->setHelpSource(value);
	}
	else if (QStringLiteral("focus") == name)
	{
	    if(w)
	    {
		QWidget *wf = w;
		if( wf->focusProxy() )
		    wf = wf->focusProxy();
		if( QStringLiteral("true") == value )
		    wf->setFocus();
		else
		    wf->clearFocus();
	    }
	}
	else if (QStringLiteral("tab-order") == name)
	{
	    QWidget *first = 0;
	    alWidget *aw = g_widgetlist->alWidgetById(value);
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
	else
	    qDebug("Ignore unknown attribute for widget <%s> <%s>=<%s>", g_enums->widgetToStr(type()).data(), qPrintable(name), qPrintable(value));
}

QSizePolicy alWidget::adjustSizePolicy(const AlteratorWidgetType type, const QSizePolicy policy, const Qt::Orientation orient)
{
    QSizePolicy szpol = policy;
    switch( type )
    {
	case WBox:
	case WVBox:
	case WHBox:
	case WTabBox:
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
	case WCheckTree:
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
	case WDisplaySize:
	case WLabel:
	{
	    szpol.setVerticalPolicy(QSizePolicy::Fixed);
	    break;
	}
	case WColorSelect:
	case WFileSelect:
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
		    {
			orientation = Qt::Vertical;
		    }
		    chld->setSizePolicy(adjustSizePolicy( type, chld->sizePolicy(), attr.value(QStringLiteral("orientation")).o));
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
	g_widgetlist->groupRemove(this);
	deleteLater();
    }
}

// non-object
alWidget* alWidgetCreateWidgetGetParent(const QString& parent)
{
    return g_widgetlist->alWidgetById(parent);
}
