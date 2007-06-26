
#include "main_window.hh"

#include "al_widget.hh"
#include "utils.hh"

extern MainWindow *main_window;
QMap<QString,alWidget*> elements;

alWidget::alWidget(Type type, const QString& id,const QString& parent):
    QObject(elements.value(parent,0)),
    type_(type),
    id_(id),
    parent_(parent),
    children_alignment(Qt::AlignJustify)
{
    elements[id] = this;
}

alWidget::~alWidget()
{
    elements.remove(id_);
}

void alWidget::onUpdate() { emit updated(); }
void alWidget::onUpdate(int) { emit updated(); }

void alWidget::onClick()     { main_window->emitEvent(id_,"clicked"); }
void alWidget::onBlockingClick()     { main_window->emitEvent(id_,"clicked", AlteratorRequestBlocking); }
void alWidget::onClick(bool) { main_window->emitEvent(id_,"clicked"); }
void alWidget::onClick(QListWidgetItem*)     { main_window->emitEvent(id_,"clicked"); }
void alWidget::onClick(QTreeWidgetItem*,int) { main_window->emitEvent(id_,"clicked"); }

void alWidget::onChange()    { main_window->emitEvent(id_,"changed"); }
void alWidget::onChange(int) { main_window->emitEvent(id_,"changed"); }
void alWidget::onChange(const QString&) { main_window->emitEvent(id_,"changed"); }
void alWidget::onChange(QWidget*)       { main_window->emitEvent(id_,"changed"); }

void alWidget::onReturn() { main_window->emitEvent(id_,"return-pressed"); }
void alWidget::onReturn(QListWidgetItem*) { main_window->emitEvent(id_,"return-pressed"); }
void alWidget::onReturn(QTreeWidgetItem*, int) { main_window->emitEvent(id_,"return-pressed"); }

void alWidget::onSelect() { main_window->emitEvent(id_,"selected"); }
void alWidget::onSelect(int n) { main_window->emitEvent(id_,"selected"); }
void alWidget::onSelect(QTreeWidgetItem*,QTreeWidgetItem*) { main_window->emitEvent(id_,"selected"); }
void alWidget::onSelect(const QString& s) { main_window->emitEvent(id_,"selected"); }

void alWidget::onDoubleClick() { main_window->emitEvent(id_,"double-clicked"); }
void alWidget::onDoubleClick(QTreeWidgetItem*,int) { main_window->emitEvent(id_,"double-clicked"); }
void alWidget::onDoubleClick(QListWidgetItem*) { main_window->emitEvent(id_,"double-clicked"); }

void alWidget::onToggle(bool) { main_window->emitEvent(id_,"toggled"); }
void alWidget::onToggle(int) { main_window->emitEvent(id_,"toggled"); }

void alWidget::setAttr(const QString& name,const QString& value)
{
	if ("visibility" == name)
		("true" == value)?getWidget()->show():getWidget()->hide();
	else if ("activity" == name)
		getWidget()->setEnabled("true" == value);
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
	    getWidget()->setToolTip(tip);
	}
	else if ("layout-policy" == name)
	{
	}
	else if ("max-width" == name)
	{
	    getWidget()->setMaximumWidth(value.toInt());
	}
	else if ("max-height" == name)
	{
	    getWidget()->setMaximumHeight(value.toInt());
	}
	else if ("width" == name)
	{
	    getWidget()->setFixedWidth(value.toInt());
	}
	else if ("height" == name)
	{
	    getWidget()->setFixedHeight(value.toInt());
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
	else if ("children-align" == name)
	{
	    // turn off
	    //setChildrenAlignment(Utils::convertAlign(value));
	}
	else if ("align" == name)
	{
	    QWidget *pw = getWidget()->parentWidget();
	    if( pw )
	    {
		QLayout *l = pw->layout();
		if( l )
		    l->setAlignment(getWidget(), Utils::convertAlign(value));
	    }
	}
	else if ("help" == name)
	{
	    main_window->setHelpSource(value);
	}
	else if ("focus" == name)
	{
	    ("false" == value)?getWidget()->clearFocus():getWidget()->setFocus();
	}
	else if ("tab-order" == name)
	{
	    QWidget *first = 0;
	    if( elements.contains(value) )
		first = elements[value]->getWidget();
	    if( first )
	    {
		QWidget *w = getWidget();
		w->setTabOrder(first, w);
	    }
	}
}

QSizePolicy alWidget::adjustSizePolicy(const Type type, const QSizePolicy policy, const Qt::Orientation parent_orientation)
{
    QSizePolicy szpol = policy;
    switch( type )
    {
	case VBox:
	{
	    szpol.setVerticalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case HBox:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case VTabBox:
	case VGroupBox:
	{
	    szpol.setVerticalPolicy(QSizePolicy::Expanding);
	}
	case HTabBox:
	case HGroupBox:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case GridBox:
	case TextBox:
	case SplitBox:
	{
	    szpol.setVerticalPolicy(QSizePolicy::Expanding);
	    szpol.setHorizontalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case Tree:
	case ListBox:
	case MultiListBox:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Preferred);
	    szpol.setVerticalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case ComboBox:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Preferred);
	    break;
	}
	case ProgressBar:
	case Slider:
	case CheckBox:
	case Radio:
	{
	    //szpol.setVerticalPolicy(QSizePolicy::Maximum);
	    //szpol.setHorizontalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case Label:
	{
	    szpol.setVerticalPolicy(QSizePolicy::Fixed);
	    break;
	}
	case Edit:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case Button:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Fixed);
	    break;
	}
	case Separator:
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
	default:
	{
	    //qDebug("don't set size policy");
	}
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
    QList<alWidget*> childs = findChildren<alWidget*>();
    QListIterator<alWidget*> it(childs);
    while(it.hasNext())
            it.next()->destroyLater();
    deleteLater();
}

void alWidget::addChild(QWidget* chld, Type type, const AlteratorRequestActionAttrs&)
{
    switch( type )
    {
	case Dialog:
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
