#include "al_tabbox.hh"
#include "widgetlist.hh"

alTabBox::alTabBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
	alWidgetPre<ATabWidget>(attr,WTabBox,id,parent)
{
}

void alTabBox::setAttr(const QString& name,const QString& value)
{
	if ("current" == name)
	{
	    wnd_->setCurrentIndex(value.toInt());
	}
	else if ("text" == name)
	{
	    QStringList values = value.split(";", QString::KeepEmptyParts);
	    if( values.size() >= 2 )
	    {
		int n = values[0].toInt();
		if( n == -1 ) n = wnd_->count() - 1;
		wnd_->setTabText(n, values[1]);
	    }
	}
	else
	    alWidget::setAttr(name,value);
}

void alTabBox::registerEvent(const QString& name)
{
       if ("selected" == name)
               connect(wnd_,SIGNAL(currentChanged(int)),SLOT(onSelect(int)));
}

QString alTabBox::postData() const
{
    return QString(" (current . %1 )").arg(wnd_->currentIndex());
}

void alTabBox::postAddChild(QWidget *chld, AlteratorWidgetType, const AlteratorRequestActionAttrs& attr)
{
    wnd_->addTab(chld, "");
}
