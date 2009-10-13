#include "al_tabbox.hh"
#include "widgetlist.hh"

alTabBox::alTabBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, Qt::Orientation orientation):
	alWidgetPre<ATabWidget>(attr,WTabBox,id,parent)
{
}

void alTabBox::setAttr(const QString& name,const QString& value)
{
	if ("current" == name)
	{
	    wnd_->setCurrentIndex(value.toInt());
	}
	if ("text" == name)
	{
	    QStringList values = value.split(";", QString::KeepEmptyParts);
	    if( values.size() >= 2 )
		wnd_->setTabText(values[0].toInt(), values[1]);
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
