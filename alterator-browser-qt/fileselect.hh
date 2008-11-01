#ifndef QT_BROWSER_FILESELECT_HH
#define QT_BROWSER_FILESELECT_HH

#include <QFileDialog>

#include "popup.hh"

class FileSelect: public Popup
{
Q_OBJECT
public:
    FileSelect(QWidget *parent, const QString &title, const QString &dir);
    ~FileSelect();

signals:
    void filesSelected(const QStringList&);

private:
    QFileDialog *file_dialog;
};

#endif
