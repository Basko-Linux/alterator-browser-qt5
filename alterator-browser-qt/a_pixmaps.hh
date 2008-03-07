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
    AStdPixmapGenerator(QStyle::StandardPixmap id):
	APixmapGenerator(APixmapGenerator::Standard),
	id_(id)
    {};
    QPixmap operator()();
};

class ANullPixmapGenerator: public APixmapGenerator
{
public:
    ANullPixmapGenerator():
	APixmapGenerator(APixmapGenerator::Generated)
	{};
    QPixmap operator()();
};

class APEButtonPixmapGenerator: public APixmapGenerator
{
public:
    APEButtonPixmapGenerator(
	QStyle::PrimitiveElement id,
	QStyle::SubElement sub,
	int options):
	APixmapGenerator(APixmapGenerator::Generated),
	    id_(id),
	    sub_(sub),
	    options_(options)
	{};
	QPixmap operator()();
private:
    QStyle::PrimitiveElement id_;
    QStyle::SubElement sub_;
    int options_;
};

class AFilePixmapGenerator: public APixmapGenerator
{
public:
    AFilePixmapGenerator(const QString& name):
	APixmapGenerator(APixmapGenerator::File),
	name_(name)
    {};
    QPixmap operator()();
private:
    QString name_;
};

extern QStringList pixmap_paths;

#endif
