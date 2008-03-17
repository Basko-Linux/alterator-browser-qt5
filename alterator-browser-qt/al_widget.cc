
#include "main_window.hh"

#include "al_widget.hh"
#include "utils.hh"
#include "widgetlist.hh"

extern MainWindow *main_window;

alWidget::alWidget(AlteratorWidgetType type, const QString& id,const QString& parent):
    QObject(widgetlist->qWidgetById(parent)),
    type_(type),
    id_(id),
    parent_(parent),
    children_alignment(Qt::AlignJustify)
{
    widgetlist->add(id, this);
}

alWidget::~alWidget()
{
    widgetlist->remove(id_);
}

void alWidget::onUpdate() { emit updated(); }
void alWidget::onUpdate(int) { emit updated(); }

void alWidget::onClick()     { main_window->emitEvent(id_, "clicked", AlteratorRequestDefault); }
void alWidget::onSpecialClick(const AlteratorRequestFlags flags)     { main_window->emitEvent(id_,"clicked", flags); }
void alWidget::onClick(bool) { main_window->emitEvent(id_,"clicked", AlteratorRequestDefault); }
void alWidget::onClick(QListWidgetItem*)     { main_window->emitEvent(id_,"clicked", AlteratorRequestDefault); }
void alWidget::onClick(QTreeWidgetItem*,int) { main_window->emitEvent(id_,"clicked", AlteratorRequestDefault); }

void alWidget::onChange()    { main_window->emitEvent(id_,"changed", AlteratorRequestDefault); }
void alWidget::onChange(int) { main_window->emitEvent(id_,"changed", AlteratorRequestDefault); }
void alWidget::onChange(const QString&) { main_window->emitEvent(id_,"changed", AlteratorRequestDefault); }
void alWidget::onChange(QWidget*)       { main_window->emitEvent(id_,"changed", AlteratorRequestDefault); }

void alWidget::onReturn() { main_window->emitEvent(id_,"return-pressed", AlteratorRequestDefault); }
void alWidget::onReturn(QListWidgetItem*) { main_window->emitEvent(id_,"return-pressed", AlteratorRequestDefault); }
void alWidget::onReturn(QTreeWidgetItem*, int) { main_window->emitEvent(id_,"return-pressed", AlteratorRequestDefault); }

void alWidget::onSelect() { main_window->emitEvent(id_,"selected", AlteratorRequestDefault); }
void alWidget::onSelect(int n) { main_window->emitEvent(id_,"selected", AlteratorRequestDefault); }
void alWidget::onSelect(QTreeWidgetItem*,QTreeWidgetItem*) { main_window->emitEvent(id_,"selected", AlteratorRequestDefault); }
void alWidget::onSelect(const QString& s) { main_window->emitEvent(id_,"selected", AlteratorRequestDefault); }

void alWidget::onDoubleClick() { main_window->emitEvent(id_,"double-clicked", AlteratorRequestDefault); }
void alWidget::onDoubleClick(QTreeWidgetItem*,int) { main_window->emitEvent(id_,"double-clicked", AlteratorRequestDefault); }
void alWidget::onDoubleClick(QListWidgetItem*) { main_window->emitEvent(id_,"double-clicked", AlteratorRequestDefault); }

void alWidget::onToggle(bool) { main_window->emitEvent(id_,"toggled", AlteratorRequestDefault); }
void alWidget::onToggle(int) { main_window->emitEvent(id_,"toggled", AlteratorRequestDefault); }

void alWidget::setAttr(const QString& name,const QString& value)
{
	QWidget *w = getWidget();

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
	else if ("window-title" == name)
	{
	    if(w)
		w->setWindowTitle(value);
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
	    QLayout *l = getViewLayout();
	    if( l )
		l->setMargin(value.toInt());
	}
	else if ("spacing" == name)
	{
	    QLayout *l = getViewLayout();
	    if( l )
		l->setSpacing(value.toInt());
	}
	else if("clear-layout" == name)
	{
	    QLayout *l = getViewLayout();
	    if( l )
	    {
		for (int i = 0; i < l->count(); ++i)
		    delete l->takeAt(i);
	    }
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
	    main_window->setHelpSource(value);
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
		w->setTabOrder(first, w);
	}
}

QSizePolicy alWidget::adjustSizePolicy(const AlteratorWidgetType type, const QSizePolicy policy, const Qt::Orientation parent_orientation)
{
    QSizePolicy szpol = policy;
    switch( type )
    {
	case WBox:
	case WVBox:
	{
	    szpol.setVerticalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case WHBox:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case WTabBox:
	case WVTabBox:
	case WGroupBox:
	case WVGroupBox:
	{
	    szpol.setVerticalPolicy(QSizePolicy::Expanding);
	}
	case WHTabBox:
	case WHGroupBox:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Expanding);
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
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Fixed);
	    break;
	}
	case WSeparator:
	{
/*
	    if( parent_orientation == Qt::Horizontal )
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

void alWidget::destroyLater()
{
    setObjectName("");
//    QList<alWidget*> childs = findChildren<alWidget*>();
    QList<alWidget*> children = widgetlist->alChildrenById(id_);
    QListIterator<alWidget*> it(children);
    while(it.hasNext())
            it.next()->destroyLater();
    deleteLater();
}

void alWidget::addChild(QWidget* chld, AlteratorWidgetType type, const AlteratorRequestActionAttrs&)
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
		    chld->setSizePolicy(adjustSizePolicy( type, chld->sizePolicy(), orientation ));
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

alWidget* createWidgetGetParent(const QString& parent)
{
    return widgetlist->alWidgetById(parent);
}
