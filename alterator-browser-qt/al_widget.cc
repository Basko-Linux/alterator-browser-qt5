
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
		getWidget()->setToolTip(args[0]);
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

QSizePolicy alWidget::adjustSizePolicy(const Type type, const QSizePolicy policy, const Qt::Orientation orientation)
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
	case SimpleBox:
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
	case ComboBox:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Preferred);
	    break;
	}
	case Button:
	{
	    szpol.setHorizontalPolicy(QSizePolicy::Fixed);
	    break;
	}
	case Separator:
	{
	    if( orientation == Qt::Horizontal )
	    {
		szpol.setHorizontalPolicy(QSizePolicy::Fixed);
		szpol.setVerticalPolicy(QSizePolicy::Expanding);
	    }
	    else
	    {
		szpol.setHorizontalPolicy(QSizePolicy::Expanding);
		szpol.setVerticalPolicy(QSizePolicy::Fixed);
	    }
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
