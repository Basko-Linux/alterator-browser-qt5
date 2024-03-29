
#include <QDialogButtonBox>

#include "fileselect.hh"

FileSelect::FileSelect(QWidget *parent, const QString &title, const QString &dir, const QString &filter):
    Popup(parent)
{
    setPopupTitle(title);
    file_dialog = new QFileDialog(view());
    file_dialog->setOptions(QFileDialog::DontUseNativeDialog);
    file_dialog->setDirectory(dir.isEmpty()? QDir::homePath(): dir);
    file_dialog->setNameFilter(filter);
    file_dialog->setVisible(true);

    QVBoxLayout *vlayout = new QVBoxLayout(view());
    vlayout->addWidget(file_dialog);

    connect(file_dialog, SIGNAL(filesSelected(const QStringList&)), this, SLOT(onSelectFiles(const QStringList&)));
}

FileSelect::~FileSelect()
{
}

void FileSelect::onSelectFiles(const QStringList &flist)
{
    selected_files = flist;
    done(QDialogButtonBox::Close);
}

QStringList FileSelect::selectedFiles()
{
    return selected_files;
}

void FileSelect::setOptions(const QString &label, QFileDialog::Options opts, QFileDialog::ViewMode view_mode, QFileDialog::FileMode file_mode, QFileDialog::AcceptMode accept_mode)
{
    file_dialog->setWindowTitle(label);
    file_dialog->setOptions(opts|QFileDialog::DontUseNativeDialog);
    file_dialog->setViewMode(view_mode);
    file_dialog->setFileMode(file_mode);
    file_dialog->setAcceptMode(accept_mode);
}
