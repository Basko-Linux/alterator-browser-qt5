#include <QMap>

#include "messagebox.hh"

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
