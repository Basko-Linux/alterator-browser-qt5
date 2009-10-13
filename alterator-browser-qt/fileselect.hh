#ifndef QT_BROWSER_FILESELECT_HH
#define QT_BROWSER_FILESELECT_HH

#include <QFileDialog>

#include "popup.hh"

class FileSelect: public Popup
{
Q_OBJECT
public:
    FileSelect(QWidget *parent, const QString &title, const QString &dir, const QString &filter);
    ~FileSelect();

    QStringList selectedFiles();
    void setOptions(const QString&, QFileDialog::Options, QFileDialog::ViewMode, QFileDialog::FileMode, QFileDialog::AcceptMode);

private Q_SLOTS:
    void onSelectFiles(const QStringList&);

private:
    QFileDialog *file_dialog;
    QStringList selected_files;
};

#endif
