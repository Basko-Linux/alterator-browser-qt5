#include <QMap>

#include "messagebox.hh"
#include "hacks.hh"

namespace AMessageBox {
	typedef QMap<QString,QMessageBox::StandardButton> buttonMapType;
	buttonMapType buttonMap;


//	typedef QMessageBox::StandardButton (*msgfunc)(QMessageBox::QWidget *,
	typedef QMessageBox::StandardButton (*msgfunc)(QWidget *,
                                       const QString &,
				       const QString &,
				       QMessageBox::StandardButtons,
				       QMessageBox::StandardButton);

	msgfunc selectMessageBox(const QString& type)
	{
		if ("critical" == type)
			return &QMessageBox::critical;
		else if ("question" == type)
			return &QMessageBox::question;
		else if ("warning" == type)
			return &QMessageBox::warning;
		else
			return &QMessageBox::information;
	}

	
	void initButtonMap()
	{
		buttonMap.insert("ok", QMessageBox::Ok);
		buttonMap.insert("open", QMessageBox::Open);
		buttonMap.insert("save",QMessageBox::Save);
		buttonMap.insert("cancel",QMessageBox::Cancel);
		buttonMap.insert("close",QMessageBox::Close);
		buttonMap.insert("discard",QMessageBox::Discard);
		buttonMap.insert("apply",QMessageBox::Apply);
		buttonMap.insert("reset",QMessageBox::Reset);
		buttonMap.insert("restore-defaults",QMessageBox::RestoreDefaults);
		buttonMap.insert("help",QMessageBox::Help);
		buttonMap.insert("save-all",QMessageBox::SaveAll);
		buttonMap.insert("yes",QMessageBox::Yes);
		buttonMap.insert("yes-to-all",QMessageBox::YesToAll);
		buttonMap.insert("no",QMessageBox::No);
		buttonMap.insert("not-to-all",QMessageBox::NoToAll);
		buttonMap.insert("abort",QMessageBox::Abort);
		buttonMap.insert("retry",QMessageBox::Retry);
		buttonMap.insert("ignore",QMessageBox::Ignore);
		buttonMap.insert("no-button",QMessageBox::NoButton);
	}

	
	QMessageBox::StandardButton convertButton(const QString& button)
	{
		return buttonMap.value(button,QMessageBox::NoButton);
	}
	
	QString	unconvertButton(QMessageBox::StandardButton button)
	{
		return buttonMap.key(button);
	}
	
	QMessageBox::StandardButtons convertButtonList(const QString& buttons)
	{
		QMessageBox::StandardButtons result = 0;
		QStringList data = buttons.split(";");
		const int len = data.size();
		for(int i = 0;i<len;++i)
			result |= convertButton(data[i]);
		return result;
	}

}

AMsgBox::AMsgBox(
	const QString &type,
	const QString &title,
	const QString &text,
	const QString &buttons,
	QWidget *parent
    ):
    QMessageBox(parent)
{
    QMessageBox::Icon i = QMessageBox::Information;
    if ("critical" == type)
	i = QMessageBox::Critical;
    else if ("question" == type)
	i = QMessageBox::Question;
    else if ("warning" == type)
	i = QMessageBox::Warning;
    setIcon(i);
    setWindowTitle(title);
    setText(text);
    QMessageBox::StandardButtons btns = AMessageBox::convertButtonList(buttons);
    if(!btns) btns |= QMessageBox::Ok;
    setStandardButtons(btns);
}

AMsgBox::~AMsgBox() {}

void AMsgBox::showEvent(QShowEvent *e)
{
    if( !main_window->haveWindowManager() )
    {
	int x = QApplication::desktop()->width()/2 - width()/2;
	if( x < 0 ) x = 0;
	int y = QApplication::desktop()->height()/2 - height()/2;
	if( y < 0 ) y = 0;
	move(x, y);
	QCursor::setPos(mapToGlobal(QPoint(width()/2, height()/2)));
    }
}

void AMsgBox::paintEvent(QPaintEvent* e)
{
    if( !main_window->haveWindowManager() )
    {
	QDialog::paintEvent(e);
	widget_corners_round(this);
    }
}
