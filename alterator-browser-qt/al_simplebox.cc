
#include <QChildEvent>

#include "al_simplebox.hh"

ASimpleBox::ASimpleBox(QWidget *parent):
    QWidget(parent)
{
    qDebug("Thank you for testing new super widget. This is experimental!!!");
    layout_type = LayoutUnknown;
    columns_ = 1;
    current_row = -1;
    current_column = 0;
    layout_ = new QGridLayout(this);
}

ASimpleBox::~ASimpleBox() {}

void ASimpleBox::appendRow()
{
    if( layout_type == LayoutColumns )
    {
	qDebug("ASimpleBox::appendRow: Ignore! You must use only one layout type");
	return;
    }
    current_row++;
    current_column = 0;
    columns_ = 1;
    layout_type = LayoutRows;
}

void ASimpleBox::setColumns(const QString &columns)
{
    if( layout_type == LayoutRows )
    {
	qDebug("ASimpleBox::setColumns: Ignore! You must use only one layout type");
	return;
    }
    else if( layout_type == LayoutColumns )
    {
	qDebug("ASimpleBox::setColumns: Ignore! Double layout setup");
	return;
    }
    QStringList cols = columns.split(";", QString::SkipEmptyParts);
    columns_ = cols.size();
    for(int i=0; i < columns_; i++)
    {
	qDebug("ASimpleBox::setColumns: %d column stretsh %d", i, cols.at(i).toInt());
	layout_->setColumnStretch(i, cols.at(i).toInt() );
    }
    if( columns_ <= 0 )
	columns_ = 1;
    current_row = 0;
    current_column = 0;
    layout_type = LayoutColumns;
    qDebug("ASimpleBox::setColumns: %d columns", columns_);
}

void ASimpleBox::childEvent(QChildEvent* e)
{
    if( e->added() )
    {
	QObject *o = e->child();
	if( o->isWidgetType() )
	{
	    QWidget *w = qobject_cast<QWidget*>(e->child());
	    if( w )
	    {
		if( layout_type == LayoutUnknown )
		{
		    qDebug("ASimpleBox::childEvent: Set default layout type by columns");
		    setColumns("100");
		}
		if( w->layout() )
		{
		    qDebug("ASimpleBox::childEvent: chiled widget already in a layout");
		    w->layout()->removeWidget(w);
		}
		qDebug("ASimpleBox::childEvent: insert at row %d, column %d", current_row, current_column);
		layout_->addWidget(w, current_row, current_column);
		if( layout_type == LayoutColumns )
		{
		    qDebug("ASimpleBox: child widget inserted by columns");
		    current_column++;
		    if( current_column >= columns_ )
		    {
			current_column = 0;
			current_row++;
		    }
		}
		else
		{
		    qDebug("ASimpleBox: child widget inserted by rows");
		    current_column++;
		}
	    }
	}
    }
}

// alSimpleBox
alSimpleBox::alSimpleBox(const QString &id,const QString &parent):
	alWidgetPre<ASimpleBox>(SimpleBox,id,parent)
{
}

void alSimpleBox::setAttr(const QString &name,const QString &value)
{
    qDebug("alSimpleBox::setAttr: %s %s", qPrintable(name), qPrintable(value));
    if ("append-text" == name)
	wnd_->appendRow();
    else if ("text" == name)
	wnd_->setColumns(value);
    else
	alWidget::setAttr(name,value);
}
