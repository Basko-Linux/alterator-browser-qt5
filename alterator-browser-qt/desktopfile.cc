
#include "browser.hh"

#include "desktopfile.hh"

DesktopFile::DesktopFile(QObject *parent, const QString &path):
    QSettings(path, QSettings::IniFormat, parent)
{
    setIniCodec("UTF-8");
    m_lng = g_browser->shortLang();
    beginGroup(QStringLiteral("Desktop Entry"));
    m_name = translatedValue(QStringLiteral("Name")).toString();
    m_comment = translatedValue(QStringLiteral("Comment")).toString();
    m_altcategory = value(QStringLiteral("X-Alterator-Category")).toString();
    m_exec = value(QStringLiteral("Exec")).toString();
}

DesktopFile::~DesktopFile()
{
}

QString DesktopFile::name()
{
    return m_name;
}

QString DesktopFile::comment()
{
    return m_comment;
}

QString DesktopFile::altCategory()
{
    return m_altcategory;
}

QString DesktopFile::exec()
{
    return m_exec;
}

QVariant DesktopFile::translatedValue(const QString &key, const QVariant &defaultValue)
{
    QVariant val(value(QString(key+QLatin1Char('[')+m_lng+QLatin1Char(']')), defaultValue));
    if( val == QVariant() )
	val = value(key, defaultValue);
    return val;
}
