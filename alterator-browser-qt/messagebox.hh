#ifndef QT_BROWSER_MESSAGE_BOX_HH
#define QT_BROWSER_MESSAGE_BOX_HH

#include <QMessageBox>

namespace AMessageBox {

typedef QMessageBox::StandardButton (*msgfunc)(QMessageBox::QWidget *,
						const QString &,
						const QString &,
						QMessageBox::StandardButtons,
						QMessageBox::StandardButton);

msgfunc selectMessageBox(const QString& type);

QMessageBox::StandardButton convertButton(const QString& button);
QString	unconvertButton(QMessageBox::StandardButton button);
QMessageBox::StandardButtons convertButtonList(const QString& buttons);


void initButtonMap();

};

#endif
