#include "al_linkbutton.hh"
#include "a_pixmaps.hh"

ALinkButton::ALinkButton(QWidget *parent, const Qt::Orientation):
    AWidget<QWidget>(parent)
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

ALinkButton::~ALinkButton()
{}

void ALinkButton::onLinkActivated(const QString&)
{
    Q_EMIT clicked();
}

void ALinkButton::setText(const QString &txt)
{
    txt_lbl->setText(QStringLiteral("<a href=\"/\">") + txt + QStringLiteral("</a>"));
}

void ALinkButton::setPixmap(const QPixmap &pix)
{
    pix_lbl->setPixmap(pix);
}

/* alLinkButton */
void alLinkButton::setAttr(const QString& name,const QString& value)
{
	if (QStringLiteral("text") == name)
		wnd_->setText(value);
	else if (QStringLiteral("pixmap") == name)
		wnd_->setPixmap(getPixmap(value));
	else
		alWidget::setAttr(name,value);
}

void alLinkButton::registerEvent(const QString& name)
{
	if (QStringLiteral("clicked") == name)
		connect(wnd_,SIGNAL( clicked() ),SLOT(onClick()));
}
