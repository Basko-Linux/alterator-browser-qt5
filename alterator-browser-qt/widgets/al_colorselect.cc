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
    connect(lineedit, SIGNAL(textEdited(const QString&)), this, SLOT(onUserEdited(const QString&)));
    connect(lineedit, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
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
    }
}

void AColorSelect::setTitle(const QString &title)
{
    dlg_title = title;
}

void AColorSelect::onUserEdited(const QString &sel)
{
    setBtnColor(sel);
    if( sel != old_txt )
    {
	old_txt = sel;
	emit changed();
    }
}

void AColorSelect::setSelected(const QString &sel, bool user)
{
    QString old_text = lineedit->text();
    lineedit->setText(sel);
    QString new_text = lineedit->text();
    setBtnColor(sel);
    if( user && old_text != new_text )
    {
	emit selected();
	emit changed();
    }
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
	wnd_->setSelected(value, false);
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
        connect(wnd_, SIGNAL(selected()), this, SLOT(onSelect()));
    else if( "changed" == name )
	connect(wnd_, SIGNAL(changed()), this, SLOT(onChange()));
    else if( "return-pressed" == name )
	connect(wnd_, SIGNAL(editingFinished()), this, SLOT(onReturn()));
}

QString alColorSelect::postData() const
{
    return QString(" (value . \"%1\" )").arg(Utils::simpleQuote(wnd_->selectedColor()));
}
