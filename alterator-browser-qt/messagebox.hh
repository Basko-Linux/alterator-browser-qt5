#ifndef QT_BROWSER_MESSAGE_BOX_HH
#define QT_BROWSER_MESSAGE_BOX_HH

#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>
#include "popup.hh"


namespace MsgBox {

QDialogButtonBox::StandardButton convertButton(const QString& button);
QString	unconvertButton(QDialogButtonBox::StandardButton button);
QDialogButtonBox::StandardButtons convertButtonList(const QString& buttons);


void initButtonMap();

}

class MessageBox: public Popup
{
Q_OBJECT
public:
    MessageBox(
	const QString &type,
	const QString &title,
	const QString &text,
	QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::Ok,
	QWidget *parent = 0
    );
    ~MessageBox();

public Q_SLOTS:
    void onButtonPressed(QAbstractButton*);
    void execForTimeout(int);

private:
    QDialogButtonBox *buttonbox;
    QVBoxLayout *layout_main;
    QLabel *iconlabel;
    QLabel *textlabel;
};

#endif
