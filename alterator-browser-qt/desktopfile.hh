#ifndef QT_BROWSER_DESKTOPFILE_HH
#define QT_BROWSER_DESKTOPFILE_HH

#include <QSettings>
//#include <QVariant>

class DesktopFile: public QSettings
{
public:
    DesktopFile(QObject *parent, const QString &path);
    ~DesktopFile();

    QString altCategory();
    QString name();
    QString comment();
    QString exec();

private:
    QString m_lng;
    QString m_name;
    QString m_altcategory;
    QString m_comment;
    QString m_exec;

    QVariant translatedValue(const QString &key, const QVariant &defaultValue = QVariant());
};

#endif
