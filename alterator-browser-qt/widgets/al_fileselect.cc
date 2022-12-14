#include <QFileDialog>

#include "al_fileselect.hh"
#include "a_pixmaps.hh"

AFileSelect::AFileSelect(QWidget *parent, const Qt::Orientation):
    AWidget<QWidget>(parent)
{
    lay = new QHBoxLayout(this);
    lineedit = new QLineEdit(this);
    btn = new QPushButton(this);
    btn->setIcon(getPixmap("theme:directory"));
    QSizePolicy pol = btn->sizePolicy();
    pol.setHorizontalPolicy(QSizePolicy::Maximum);
    btn->setSizePolicy(pol);

    lay->addWidget(lineedit);
    lay->addWidget(btn);

    connect(btn, SIGNAL(clicked()), this, SLOT(showDialog()));
    connect(lineedit, SIGNAL(textEdited(const QString&)), this, SLOT(onUserEdited(const QString&)));
    connect(lineedit, SIGNAL(editingFinished()), this, SIGNAL(editingFinished()));
}

AFileSelect::~AFileSelect() {}

void AFileSelect::onUserEdited(const QString &sel)
{
    if( !sel.isEmpty() && sel != old_txt )
    {
	old_txt = sel;
	Q_EMIT changed();
    }
}

void AFileSelect::setOptions(const QString &opts)
{
    dlg_opts = opts;
}

void AFileSelect::setTitle(const QString &title)
{
    dlg_title = title;
}

void AFileSelect::setDir(const QString &dir)
{
    dlg_dir = dir;
}

void AFileSelect::setFilter(const QString &filter)
{
    dlg_filter = filter;
}

void AFileSelect::setSelected(const QString &sel, bool user)
{
    lineedit->setText(sel);
    if( user )
	Q_EMIT selected();
}

QString AFileSelect::selectedFiles()
{
    return lineedit->text();
}

void AFileSelect::showDialog()
{
    QFileDialog dialog(this);
    QFileDialog::Options options = 0;
    QFileDialog::ViewMode view_mode = QFileDialog::Detail;
    QFileDialog::FileMode file_mode = QFileDialog::AnyFile;
    QFileDialog::AcceptMode accept_mode = QFileDialog::AcceptOpen;
    Q_FOREACH(QString option, dlg_opts.split(QLatin1String(";"), QT_SKIPEMPTYPARTS))
    {
	if( option == "show_dirs_only" )
	    { options = options | QFileDialog::ShowDirsOnly; }
	else if( option == "dont_resolve_symlinks" )
	    { options = options | QFileDialog::DontResolveSymlinks; }
	else if( option == "dont_confirm_overwrite" )
	    { options = options | QFileDialog::DontConfirmOverwrite; }
	else if( option == "read_only" )
	    { options = options | QFileDialog::ReadOnly; }
	else if( option == "hide_name_filter_details" )
	    { options = options | QFileDialog::HideNameFilterDetails; }
	else if( option == "detail" )
	    { view_mode = QFileDialog::Detail; }
	else if( option == "list" )
	    { view_mode = QFileDialog::List; }
	else if( option == "any_file" )
	    { file_mode = QFileDialog::AnyFile; }
	else if( option == "existing_file" )
	    { file_mode = QFileDialog::ExistingFile; }
	else if( option == "directory" )
	    { file_mode = QFileDialog::Directory; }
	else if( option == "existing_files" )
	    { file_mode = QFileDialog::ExistingFiles; }
	else if( option == "accept_open" )
	    { accept_mode = QFileDialog::AcceptOpen; }
	else if( option == "accept_save" )
	    { accept_mode = QFileDialog::AcceptSave; }
    }
    dialog.setWindowTitle(dlg_title);
    dialog.setDirectory(dlg_dir.isEmpty()? QDir::homePath(): dlg_dir);
    dialog.setNameFilter(dlg_filter);
    dialog.setOptions(options|QFileDialog::DontUseNativeDialog);
    dialog.setViewMode(view_mode);
    dialog.setFileMode(file_mode);
    dialog.setAcceptMode(accept_mode);
    if( dialog.exec() == QDialog::Accepted )
    {
	setSelected(dialog.selectedFiles().join(";"));
    }
}

// alFileSelect
alFileSelect::alFileSelect(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent):
    alWidgetPre<AFileSelect>(attr,WFileSelect,id,parent)
{
}

void alFileSelect::setAttr(const QString& name,const QString& value)
{
    if ("url" == name)
    {
	wnd_->setDir(value);
    }
    else if ("title" == name)
    {
	wnd_->setTitle(value);
    }
    else if ("filter" == name)
    {
	wnd_->setFilter(value);
    }
    else if ("hints" == name)
    {
	wnd_->setOptions(value);
    }
    else if ("value" == name)
    {
	wnd_->setSelected(value, false);
    }
    else
	alWidget::setAttr(name,value);
}

void alFileSelect::registerEvent(const QString& name)
{
    if ("selected" == name)
	connect(wnd_,SIGNAL( selected() ),SLOT(onSelect()));
    else if( "changed" == name )
	connect(wnd_, SIGNAL(changed()), this, SLOT(onChange()));
    else if( "return-pressed" == name )
	connect(wnd_, SIGNAL(editingFinished()), this, SLOT(onReturn()));
}

QString alFileSelect::postData() const
{
    return QString(" (value . \"%1\" )").arg(Utils::simpleQuote(wnd_->selectedFiles()));
}
