#ifndef QT_BROWSER_UTILS_HH
#define QT_BROWSER_UTILS_HH

#include <QString>
#include <QLineEdit>

namespace Utils
{
    QString reparentTag(const QString &parent);
    Qt::Alignment convertAlign(const QString& value);
    QLineEdit::EchoMode convertEchoMode(const QString& value);
    QString simpleQuote(const QString&);
    void errorExit(QWidget *parent, const QString& message);
    void widgetCornersRound(QWidget*);
    void fixWmlessPopup(QWidget*);
    Qt::Orientation fixOrientation(Qt::Orientation value, Qt::Orientation def_value = Qt::Vertical);
    Qt::Orientation convertOrientation(const QString&);
}

#endif
