#include <QMap>
#include <QApplication>
#include <QHBoxLayout>

#include "messagebox.hh"
#include "browser.hh"
#include "utils.hh"

namespace MsgBox {
	typedef QMap<QString,QDialogButtonBox::StandardButton> buttonMapType;
	buttonMapType buttonMap;


	void initButtonMap()
	{
		buttonMap.insert("ok", QDialogButtonBox::Ok);
		buttonMap.insert("open", QDialogButtonBox::Open);
		buttonMap.insert("save",QDialogButtonBox::Save);
		buttonMap.insert("cancel",QDialogButtonBox::Cancel);
		buttonMap.insert("close",QDialogButtonBox::Close);
		buttonMap.insert("discard",QDialogButtonBox::Discard);
		buttonMap.insert("apply",QDialogButtonBox::Apply);
		buttonMap.insert("reset",QDialogButtonBox::Reset);
		buttonMap.insert("restore-defaults",QDialogButtonBox::RestoreDefaults);
		buttonMap.insert("help",QDialogButtonBox::Help);
		buttonMap.insert("save-all",QDialogButtonBox::SaveAll);
		buttonMap.insert("yes",QDialogButtonBox::Yes);
		buttonMap.insert("yes-to-all",QDialogButtonBox::YesToAll);
		buttonMap.insert("no",QDialogButtonBox::No);
		buttonMap.insert("no-to-all",QDialogButtonBox::NoToAll);
		buttonMap.insert("abort",QDialogButtonBox::Abort);
		buttonMap.insert("retry",QDialogButtonBox::Retry);
		buttonMap.insert("ignore",QDialogButtonBox::Ignore);
		buttonMap.insert("no-button",QDialogButtonBox::NoButton);
	}

	
	QDialogButtonBox::StandardButton convertButton(const QString& button)
	{
		return buttonMap.value(button,QDialogButtonBox::NoButton);
	}
	
	QString	unconvertButton(QDialogButtonBox::StandardButton button)
	{
		return buttonMap.key(button);
	}
	
	QDialogButtonBox::StandardButtons convertButtonList(const QString& buttons)
	{
		QDialogButtonBox::StandardButtons result = 0;
		QStringList data = buttons.split(QLatin1String(";"));
		const int len = data.size();
		for(int i = 0;i<len;++i)
			result |= convertButton(data[i]);
		return result;
	}

}

MessageBox::MessageBox(
	const QString &type,
	const QString &title,
	const QString &text,
	QDialogButtonBox::StandardButtons buttons,
	QWidget *parent
    ):
    Popup(parent, true)
{
    //qDebug("new MessageBox");
    QStyle::StandardPixmap pix_id = QStyle::SP_MessageBoxInformation;
    if ("critical" == type) pix_id = QStyle::SP_MessageBoxCritical;
    else if ("question" == type) pix_id = QStyle::SP_MessageBoxQuestion;
    else if ("warning" == type) pix_id = QStyle::SP_MessageBoxWarning;
    iconlabel = new QLabel(this);
    iconlabel->setSizePolicy(QSizePolicy::Maximum,QSizePolicy::Maximum);
    iconlabel->setPixmap(QApplication::style()->standardIcon(pix_id).pixmap(32,32));
    textlabel = new QLabel(this);
    textlabel->setWordWrap(true);
    textlabel->setAlignment(Qt::AlignLeft);
    textlabel->setText(text);
    buttonbox = new QDialogButtonBox(this);
    buttonbox->setStandardButtons(buttons);

    layout_main = new QVBoxLayout(view());
    QHBoxLayout *msg_layout = new QHBoxLayout();
    layout_main->addLayout(msg_layout);
    msg_layout->addWidget(iconlabel);
    msg_layout->addWidget(textlabel);
    layout_main->addWidget(buttonbox);

    setPopupTitle(title);

    connect(buttonbox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(onButtonPressed(QAbstractButton*)));
}

MessageBox::~MessageBox()
{
}

void MessageBox::onButtonPressed(QAbstractButton *btn)
{
    done(buttonbox->standardButton(btn));
}

void MessageBox::execForTimeout(int msec)
{
    QTimer::singleShot(msec, this, &Popup::accept);
    exec();
    g_browser->popupRemove(this);
}
