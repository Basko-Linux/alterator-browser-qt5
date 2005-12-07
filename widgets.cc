#include "widgets.hh"

////////////////////////////////////////////

Qt::Alignment convertAlign(const QString& value)
{
	if ("left" == value) return  Qt::AlignLeft;
	else if ("right" == value) return Qt::AlignRight;
	else if ("center" == value) return Qt::AlignCenter;
	return Qt::AlignCenter;
}

QLineEdit::EchoMode convertEchoMode(const QString& value)
{
	if ("no" == value) return QLineEdit::NoEcho;
	else if ("stars" == value) return QLineEdit::Password;
	return QLineEdit::Normal; //default yes
}

////////////////////////////////////////////

//all current elements on viewer
QMap<QString,alWidget*> elements;

#include <iostream>

MyBoxLayout *getLayout(const QString& parent)
{
	if (!elements.contains(parent)) return 0;

	QLayout *l=elements[parent]->getWidget()->layout();
	return l?static_cast<MyBoxLayout*>(l):0;
}

void alWidget::setAttr(const QString& name,const QString& value)
{
	if ("visibility" == name)
		(value == "yes")?getWidget()->show():getWidget()->hide();
	else if ("activity" == name)
		getWidget()->setEnabled("yes" == value);
	else if ("widget-name" == name)
		getWidget()->setObjectName(value);
	else if ("layout-policy" == name)
	{
		QStringList policies = value.split(";");
		if (policies.count() < 2)
		{
			qDebug("wrong number of arguments to layout-policy attribute");
			exit(1);
		}
		int w = policies.at(0).toInt();
		int h = policies.at(1).toInt();
		int d = -1;
		if (policies.count() > 2) d = convertAlign(policies.at(2));
		MyBoxLayout *playout = getLayout(parent_);
		if (playout) playout->addWidget(getWidget(),QSize(w,h),d);
	}
}

////////////////////////////////////////////
void alLabel::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("pixmap" == name)
		wnd_->setPixmap(QPixmap(value));
	else
		alWidget::setAttr(name,value);
}

void alButton::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("pixmap" == name)
		wnd_->setIcon(QIcon(value));
	else
		alWidget::setAttr(name,value);
}

void alButton::registerEvent(const QString& name)
{
	if ("on-click" == name)
		connect(wnd_,SIGNAL( clicked(bool) ),SLOT(onClick(bool)));
}



void alEdit::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("echo" == name) 
		wnd_->setEchoMode(convertEchoMode(value));
	else if ("alterability" == name)
		wnd_->setReadOnly(value == "no");
	else
		alWidget::setAttr(name,value);
}

void alEdit::registerEvent(const QString& name)
{
	if ("on-change" == name)
		connect(wnd_,SIGNAL( textEdited(const QString&) ),SLOT(onChange(const QString&)));
}


QString alEdit::postData() const
{
	return " (text . \""+simpleQuote(wnd_->text())+"\" )";
}

void alDialog::setAttr(const QString& name,const QString& value)
{
	if ("caption" == name)
		wnd_->setWindowTitle(value);
	else
		alWidget::setAttr(name,value);
}

void alBox::setAttr(const QString& name,const QString& value)
{
	if ("margin" == name)
		wnd_->layout()->setMargin(value.toInt());
	else if ("spacing" == name)
		wnd_->layout()->setSpacing(value.toInt());
	else if("clear-layout" == name)
		((MyBoxLayout*)(wnd_->layout()))->deleteAllItems();
	else if ("children-align" == name)
		((MyBoxLayout*)(wnd_->layout()))->setChildrenAlign(convertAlign(value));
	else alWidget::setAttr(name,value);
}
