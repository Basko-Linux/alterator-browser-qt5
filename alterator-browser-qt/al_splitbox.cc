
#include "al_splitbox.hh"

ASplitBox::ASplitBox(QWidget *parent):
    QSplitter(parent)
{
}

ASplitBox::~ASplitBox() {}

void ASplitBox::addChild(QWidget* chld)
{
    addWidget(chld);
}

// alSplitBox
alSplitBox::alSplitBox(const QString &id,const QString &parent, Qt::Orientation orientation):
	alWidgetPre<ASplitBox>(SplitBox,id,parent)
{
    wnd_->setOrientation(Utils::fixOrientation(orientation, Qt::Horizontal));
}

void alSplitBox::addChild(QWidget *chld, alWidget::Type type)
{
    wnd_->addChild(chld);
}
