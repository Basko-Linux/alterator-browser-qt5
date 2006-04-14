#ifndef QTBROWSER_APIXMAPS_HH
#define QTBROWSER_APIXMAPS_HH

#include <QString>
#include <QPixmap>
#include <QMap>
#include <QStyle>

QPixmap getPixmap(QString id);

class APixmapGenerator
{
public:
    enum Type
    {
	Generated,
	Standard,
	File
    };
    APixmapGenerator(Type type): type_(type) {};
    virtual ~APixmapGenerator() {};

    virtual QPixmap operator()() = 0;
    Type type() {return type_;};
private:
    Type type_;
};

typedef QMap<QString,APixmapGenerator*> pix_map_t;

class AStdPixmapGenerator: public APixmapGenerator
{
    QStyle::StandardPixmap id_;
public:
    AStdPixmapGenerator(QStyle::StandardPixmap id);
    QPixmap operator()();
};

class ANullPixmapGenerator: public APixmapGenerator
{
public:
	ANullPixmapGenerator();
	QPixmap operator()();
};

class APEButtonPixmapGenerator: public APixmapGenerator
{
public:
	APEButtonPixmapGenerator(
	    QStyle::PrimitiveElement id,
	    QStyle::SubElement sub,
	    QStyle::StateFlag options);
	QPixmap operator()();
private:
    QStyle::PrimitiveElement id_;
    QStyle::SubElement sub_;
    QStyle::StateFlag options_;
};

class AFilePixmapGenerator: public APixmapGenerator
{
public:
	AFilePixmapGenerator(const QString& name);
	QPixmap operator()();
private:
    QString name_;
};


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
