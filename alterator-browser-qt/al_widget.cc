
#include "al_widget.hh"

QMap<QString,alWidget*> elements;

alWidget::alWidget(const QString& id,const QString& parent):
    QObject(elements.value(parent,0)),
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
	    /*
		QStringList policies = value.split(";");
		if (policies.count() < 2)
		{
		    qDebug("wrong number of arguments to layout-policy attribute");
		    exit(1);
		}

		QLayout *playout = findViewLayout(parent_);
		if( playout )
		{
		    if( MyBoxLayout *ml = qobject_cast<MyBoxLayout*>(playout) )
		    {
			int w = policies.at(0).toInt();
			int h = policies.at(1).toInt();
			int d = -1;
			if (policies.count() > 2)
			    d = convertAlign(policies.at(2));
			ml->addWidget(getWidget(),QSize(w,h),d);
		    }
		    else
			qDebug("don't set layout-policy");
		}
		else
		    qDebug("no layout");
		*/
	}
	else if ("help" == name)
	{
		//updateHelp(value);
	}
}

