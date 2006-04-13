#ifndef QTBROWSER_APIXMAPS_HH
#define QTBROWSER_APIXMAPS_HH

#include <QString>
#include <QPixmap>
#include <QMap>
#include <QStyle>

class APixmaps
{
public:
    APixmaps();
    ~APixmaps();

    QPixmap get(const QString &id);

private:
    QMap<QString, QStyle::StandardPixmap> standard;
    QMap<QString, QPair<QStyle::PrimitiveElement, int> > generated;
    QMap<QString, QString> filesystem;

    QPixmap generate(const QString &id);
};

#endif
