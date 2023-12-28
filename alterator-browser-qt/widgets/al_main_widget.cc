
#include "al_main_widget.hh"

AMainWidget::AMainWidget(QWidget *parent, const Qt::Orientation):
    AWidget<QFrame>(parent)
{
    setObjectName(QStringLiteral("main_widget"));
    setLineWidth(0);
    setFrameStyle(QFrame::Plain | QFrame::NoFrame);
    QVBoxLayout *bl = new QVBoxLayout(this);
    bl->setSpacing(5);
    bl->setContentsMargins(0,0,0,0);
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
	if (QStringLiteral("width") == name)
	{
		//wnd_->resize(value.toInt(),wnd_->height());
	}
	else if (QStringLiteral("height") == name)
	{
		//wnd_->resize(wnd_->width(),value.toInt());
	}
        else if (QStringLiteral("has-help") == name)
        {
            g_browser->setHelpAvailable(value != QStringLiteral("false"));
        }
	else
		alWidget::setAttr(name,value);
}
