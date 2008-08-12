
#include "al_main_widget.hh"

AMainWidget::AMainWidget(QWidget *parent, const Qt::Orientation):
    QWidget(parent)
{
    setObjectName("main_widget");
    QVBoxLayout *bl = new QVBoxLayout(this);
    bl->setSpacing(5);
    bl->setMargin(5);
    main_window->setCentralWidget(this);
}

AMainWidget::~AMainWidget()
{}

// alMainWidget
alMainWidget::alMainWidget(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
	alWidgetPre<AMainWidget>(attr,WMainWidget,id,parent)
{
}

void alMainWidget::setAttr(const QString& name,const QString& value)
{
	if ("width" == name)
	{
		//wnd_->resize(value.toInt(),wnd_->height());
	}
	else if ("height" == name)
	{
		//wnd_->resize(wnd_->width(),value.toInt());
	}
        else if ("has-help" == name)
        {
            main_window->setHelpAvailable(value != "false");
        }
	else
		alWidget::setAttr(name,value);
}
