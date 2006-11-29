
#include "al_widget.hh"

QMap<QString,alWidget*> elements;

alWidget::alWidget(Type type, const QString& id,const QString& parent):
    QObject(elements.value(parent,0)),
    type_(type),
    id_(id),
    parent_(parent)
{
    elements[id] = this;
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
		getWidget()->setObjectName(value);
	else if ("tooltip" == name)
	{
		QStringList args = value.split(";");
		getWidget()->setToolTip(args[0]);
	}
	else if ("layout-policy" == name)
	{
	}
	else if ("help" == name)
	{
		//updateHelp(value);
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
	case VGroupBox:
	case HGroupBox:
	{
	    szpol.setVerticalPolicy(QSizePolicy::Expanding);
	    //szpol.setHorizontalPolicy(QSizePolicy::Expanding);
	    break;
	}
	case TabBox:
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
	case Splitter:
	case CheckBox:
	case Radio:
	case Label:
	{
	    //szpol.setVerticalPolicy(QSizePolicy::Maximum);
	    //szpol.setHorizontalPolicy(QSizePolicy::Expanding);
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
	    szpol.setHorizontalPolicy(QSizePolicy::Preferred);
	    break;
	}
	default:
	{
	    //qDebug("don't set size policy");
	}
    }
    return szpol;
}
