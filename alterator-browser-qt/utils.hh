#ifndef QT_BROWSER_UTILS_HH
#define QT_BROWSER_UTILS_HH

#include <QString>
#include <QLineEdit>

namespace Utils
{
    QString reparentTag(QString parent);
    Qt::Alignment convertAlign(const QString& value);
    QLineEdit::EchoMode convertEchoMode(const QString& value);
    QString& simpleQuote(const QString&);
};

#endif
