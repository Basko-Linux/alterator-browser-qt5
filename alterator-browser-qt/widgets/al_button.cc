#include "al_button.hh"
#include "a_pixmaps.hh"

AButton::AButton(QWidget *parent, const Qt::Orientation):
    QWidget(parent)
{
    pix_lbl = new QLabel(this);
    pix_lbl->setFixedSize(16,16);
    pix_lbl->setScaledContents(true);
    txt_lbl = new QLabel(this);
    txt_lbl->setTextFormat(Qt::RichText);
    txt_lbl->setWordWrap(false);

    QHBoxLayout *main_layout = new QHBoxLayout(this);
    main_layout->addWidget(pix_lbl);
    main_layout->addWidget(txt_lbl);

    connect(txt_lbl, SIGNAL(linkActivated(const QString&)), this, SLOT(onLinkActivated(const QString&)));
}

AButton::~AButton()
{}

void AButton::onLinkActivated(const QString&)
{
    emit clicked();
}

void AButton::setText(const QString &txt)
{
    txt_lbl->setText("<a href=\"/\">" + txt + "</a>");
}

void AButton::setPixmap(const QPixmap &pix)
{
    pix_lbl->setPixmap(pix);
}

/* alButton */
void alButton::setAttr(const QString& name,const QString& value)
{
	if ("text" == name)
		wnd_->setText(value);
	else if ("pixmap" == name)
		wnd_->setPixmap(getPixmap(value));
	else
		alWidget::setAttr(name,value);
}

void alButton::registerEvent(const QString& name)
{
	if ("clicked" == name)
		connect(wnd_,SIGNAL( clicked() ),SLOT(onClick()));
}
