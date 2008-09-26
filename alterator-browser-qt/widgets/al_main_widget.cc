
#include "al_main_widget.hh"

AMainWidget::AMainWidget(QWidget *parent, const Qt::Orientation):
    QFrame(parent)
{
    setObjectName("main_widget");
    setLineWidth(0);
    setFrameStyle(QFrame::Plain | QFrame::NoFrame);
    QVBoxLayout *bl = new QVBoxLayout(this);
    bl->setSpacing(5);
    bl->setMargin(0);
    browser->setCentralWidget(this);
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
            browser->setHelpAvailable(value != "false");
        }
	else
		alWidget::setAttr(name,value);
}
