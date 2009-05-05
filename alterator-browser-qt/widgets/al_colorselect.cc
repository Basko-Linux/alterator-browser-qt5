#include <QColorDialog>

#include "al_colorselect.hh"
#include "a_pixmaps.hh"

AColorSelect::AColorSelect(QWidget *parent, const Qt::Orientation):
    AWidget<QWidget>(parent)
{
    setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);

    lay = new QHBoxLayout(this);
    lineedit = new QLineEdit(this);
    lineedit->setMaxLength(7);
    lineedit->setInputMask(">\\#HHHHHH");
    QSizePolicy pol = lineedit->sizePolicy();
    pol.setHorizontalPolicy(QSizePolicy::Maximum);
    lineedit->setSizePolicy(pol);
    btn = new QPushButton(this);
    btn->setIcon(getPixmap("theme:up"));
    pol = btn->sizePolicy();
    pol.setHorizontalPolicy(QSizePolicy::Maximum);
    btn->setSizePolicy(pol);

    lay->addWidget(lineedit);
    lay->addWidget(btn);

    connect(btn, SIGNAL(clicked()), this, SLOT(showDialog()));
    connect(lineedit, SIGNAL(textEdited(const QString&)), this, SLOT(setBtnColor(const QString&)));
}

AColorSelect::~AColorSelect() {}

void AColorSelect::setBtnColor(const QString &txtcolor)
{
    QColor clr(txtcolor);
    if(clr.isValid())
    {
	QPalette pal = btn->palette();
	pal.setColor(QPalette::Button, clr);
	btn->setPalette(pal);
    }
}

void AColorSelect::showDialog()
{
    QColorDialog dialog(this);
    dialog.setWindowTitle(dlg_title);
    dialog.setCurrentColor(QColor(lineedit->text()));
    if( dialog.exec() == QDialog::Accepted )
    {
	setSelected(dialog.selectedColor().name().toUpper());
	emit selected();
    }
}

void AColorSelect::setTitle(const QString &title)
{
    dlg_title = title;
}

void AColorSelect::setSelected(const QString &sel)
{
    lineedit->setText(sel);
    setBtnColor(sel);
}

QString  AColorSelect::selectedColor()
{
    return lineedit->text();
}

// alColorSelect
alColorSelect::alColorSelect(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
    alWidgetPre<AColorSelect>(attr,WColorSelect,id,parent)
{
}

void alColorSelect::setAttr(const QString& name,const QString& value)
{
    if ("value" == name)
    {
	wnd_->setSelected(value);
    }
    else if ("title" == name)
    {
	wnd_->setTitle(value);
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
    return QString(" (value . \"%1\" )").arg(Utils::simpleQuote(wnd_->selectedColor()));
}
