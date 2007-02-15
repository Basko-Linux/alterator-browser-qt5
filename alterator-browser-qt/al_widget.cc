
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

    cursors["arrow"] = Qt::ArrowCursor;
    cursors["arrow-up"] = Qt::UpArrowCursor;
    cursors["cross"] = Qt::CrossCursor;
    cursors["wait"] = Qt::WaitCursor;
    cursors["caret"] = Qt::IBeamCursor;
    cursors["size-vertical"] = Qt::SizeVerCursor;
    cursors["size-horizontal"] = Qt::SizeHorCursor;
    cursors["size-diagonal-forward"] = Qt::SizeBDiagCursor;
    cursors["size-diagonal-back"] = Qt::SizeFDiagCursor;
    cursors["size-all"] = Qt::SizeAllCursor;
    cursors["blank"] = Qt::BlankCursor;
    cursors["forbidden"] = Qt::ForbiddenCursor;
    cursors["split-vertical"] = Qt::SplitVCursor;
    cursors["split-horizontal"] = Qt::SplitHCursor;
    cursors["hand-pointing"] = Qt::PointingHandCursor;
    cursors["hand-open"] = Qt::OpenHandCursor;
    cursors["hand-closed"] = Qt::ClosedHandCursor;
    cursors["what-is-this"] = Qt::WhatsThisCursor;
    cursors["busy"] = Qt::BusyCursor;
}

alWidget::~alWidget()
{
    elements.remove(id_);
}

void alWidget::onUpdate(int) { emit updated(); }

void alWidget::onClick() { emitEvent(id_,"clicked"); }
void alWidget::onClick(bool) { emitEvent(id_,"clicked"); }
void alWidget::onClick(QListWidgetItem*) { emitEvent(id_,"clicked"); }
void alWidget::onClick(QTreeWidgetItem*,int) { emitEvent(id_,"clicked"); }

void alWidget::onChange() { emitEvent(id_,"changed"); }
void alWidget::onChange(int) { emitEvent(id_,"changed"); }
void alWidget::onChange(const QString&) { emitEvent(id_,"changed"); }
void alWidget::onChange(QWidget*) { emitEvent(id_,"changed"); }

void alWidget::onReturn() { emitEvent(id_,"return-pressed"); }
void alWidget::onReturn(QListWidgetItem*) { emitEvent(id_,"return-pressed"); }
void alWidget::onReturn(QTreeWidgetItem*, int) { emitEvent(id_,"return-pressed"); }

void alWidget::onSelect() { emitEvent(id_,"selected"); }
void alWidget::onSelect(int n) { emitEvent(id_,"selected"); }
void alWidget::onSelect(QTreeWidgetItem*,QTreeWidgetItem*) { emitEvent(id_,"selected"); }
void alWidget::onSelect(const QString& s) { emitEvent(id_,"selected"); }

void alWidget::onDoubleClick() { emitEvent(id_,"double-clicked"); }
void alWidget::onDoubleClick(QTreeWidgetItem*,int) { emitEvent(id_,"double-clicked"); }
void alWidget::onDoubleClick(QListWidgetItem*) { emitEvent(id_,"double-clicked"); }

void alWidget::onToggle(bool) { emitEvent(id_,"toggled"); }
void alWidget::onToggle(int) { emitEvent(id_,"toggled"); }

void alWidget::setAttr(const QString& name,const QString& value)
{
	qDebug("alWidget::setAttr");
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
	    qDebug("max-width %s", qPrintable(value));
	    getWidget()->setMaximumWidth(value.toInt());
	}
	else if ("max-height" == name)
	{
	    qDebug("max-height %s", qPrintable(value));
	    getWidget()->setMaximumHeight(value.toInt());
	}
	else if ("width" == name)
	{
	    qDebug("width %s", qPrintable(value));
	    //getWidget()->setFixedWidth(value.toInt());
	    getWidget()->setMinimumWidth(value.toInt());
	    getWidget()->setMaximumWidth(value.toInt());
	}
	else if ("height" == name)
	{
	    qDebug("height %s", qPrintable(value));
	    //getWidget()->setFixedHeight(value.toInt());
	    getWidget()->setMinimumHeight(value.toInt());
	    getWidget()->setMaximumHeight(value.toInt());
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
	else if ("cursor" == name)
	{
	    getWidget()->setCursor(cursors[value]);
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

void alWidget::addChild(QWidget* chld, Type type)
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
