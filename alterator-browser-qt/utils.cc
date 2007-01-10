#include <QMessageBox>
#include <QApplication>

#include "utils.hh"


namespace Utils
{

QString reparentTag(const QString &parent)
{
    QString ret(parent);
    return ret.replace(QString(":reparent:"),QString(""));
}

Qt::Alignment convertAlign(const QString& value)
{
	if ("left" == value) return  Qt::AlignLeft;
	else if ("right" == value) return Qt::AlignRight;
	else if ("center" == value) return Qt::AlignCenter;
	return Qt::AlignJustify;
}

QLineEdit::EchoMode convertEchoMode(const QString& value)
{
	if ("no" == value) return QLineEdit::NoEcho;
	else if ("stars" == value) return QLineEdit::Password;
	return QLineEdit::Normal; //default yes
}

QString simpleQuote(const QString &s)
{
    QString ret(s);
    return ret.replace("\\","\\\\").replace("\"","\\\"");
}

void errorExit(QWidget *parent, const QString& message)
{
    qDebug("error exit:%s",qPrintable(message));
    QMessageBox::critical(parent, QMessageBox::tr("Error"),
	message,
	QMessageBox::Abort);
    //QApplication::closeAllWindows();
    exit(1);
}

}
