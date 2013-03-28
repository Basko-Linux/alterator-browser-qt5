
#include "browser.hh"

#include "desktopfile.hh"

DesktopFile::DesktopFile(QObject *parent, const QString &path):
    QSettings(path, QSettings::IniFormat, parent)
{
    setIniCodec("UTF-8");
    m_lng = browser->shortLang();
    beginGroup("Desktop Entry");
    m_name = translatedValue("Name").toString();
    m_comment = translatedValue("Comment").toString();
    m_altcategory = value("X-Alterator-Category").toString();
    m_exec = value("Exec").toString();
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
    QVariant val(value(QString(key+"["+m_lng+"]"), defaultValue));
    if( val == QVariant() )
	val = value(key, defaultValue);
    return val;
}
