#include <QColorDialog>

#include "al_colorselect.hh"
#include "a_pixmaps.hh"

AColorSelect::AColorSelect(QWidget *parent, const Qt::Orientation):
    AWidget<QWidget>(parent)
{
}

AColorSelect::~AColorSelect() {}

void AColorSelect::showDialog()
{
    //QColorDialog dialog()
}

// alColorSelect
alColorSelect::alColorSelect(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
    alWidgetPre<AColorSelect>(attr,WColorSelect,id,parent)
{
}

void alColorSelect::setAttr(const QString& name,const QString& value)
{
    if ("text" == name)
    {
    }
    else if ("title" == name)
    {
    }
    else
	alWidget::setAttr(name,value);
}

void alColorSelect::registerEvent(const QString& name)
{
    if ("selected" == name)
    {
        connect(wnd_,SIGNAL( selected() ),SLOT(onSelect()));
    }
}

QString alColorSelect::postData() const
{
	QString post;
/*
	post = QString(" (current . %1 )").arg(wnd_->currentIndex());
	if (wnd_->isEditable() && (counter_ != wnd_->count()))
	{//reset items on alterator
	    post += "( rows . (";
	    for (int i=0;i<wnd_->count();++i)
		post += " #(( \""+ Utils::simpleQuote(wnd_->itemText(i))+ "\" . \"\"))";
	    post += "))";
	}
	post += QString(" (current-text . \"%1\" )").arg(Utils::simpleQuote(wnd_->currentText()));
*/
	return post;
}
