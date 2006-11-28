
#include "utils.hh"


namespace Utils
{

QString reparentTag(QString parent)
{
       return parent.replace(QString(":reparent:"),QString(""));
}

Qt::Alignment convertAlign(const QString& value)
{
	if ("left" == value) return  Qt::AlignLeft;
	else if ("right" == value) return Qt::AlignRight;
	else if ("center" == value) return Qt::AlignCenter;
	return Qt::AlignCenter;
}

QLineEdit::EchoMode convertEchoMode(const QString& value)
{
	if ("no" == value) return QLineEdit::NoEcho;
	else if ("stars" == value) return QLineEdit::Password;
	return QLineEdit::Normal; //default yes
}

}
