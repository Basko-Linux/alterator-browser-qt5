
#include "fileselect.hh"

FileSelect::FileSelect(QWidget *parent, const QString &title, const QString &dir):
    Popup(parent)
{
    setPopupTitle(title);
    file_dialog = new QFileDialog(this);
    file_dialog->setFileMode(QFileDialog::AnyFile);
    file_dialog->setReadOnly(true);
    file_dialog->setAcceptMode(QFileDialog::AcceptOpen);
    file_dialog->setDirectory(dir.isEmpty()? QDir::homePath(): dir);
    file_dialog->setViewMode(QFileDialog::Detail);

    QVBoxLayout *vlayout = new QVBoxLayout(this);
    vlayout->addWidget(file_dialog);

    connect(file_dialog, SIGNAL(filesSelected(const QStringList&)), this, SIGNAL(filesSelected(const QStringList&)));
}

FileSelect::~FileSelect()
{
}
