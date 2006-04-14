#include <QPixmapCache>
#include <QApplication>
#include <QPainter>
#include <QRect>
#include <QStyleOptionButton>
#include <QBitmap>

#include "a_pixmaps.hh"

#define IMAGES_PATH "/usr/share/alterator/images/"

static QString images_path(IMAGES_PATH);
static pix_map_t pix_map;
bool pix_map_initialised = false;

AStdPixmapGenerator::AStdPixmapGenerator(QStyle::StandardPixmap id):
    APixmapGenerator(APixmapGenerator::Standard),
    id_(id)
{}

QPixmap AStdPixmapGenerator::operator()()
{
	return QApplication::style()->standardPixmap(id_);
}

ANullPixmapGenerator::ANullPixmapGenerator():
    APixmapGenerator(APixmapGenerator::Generated)
{}

QPixmap ANullPixmapGenerator::operator()()
{
    QPixmap pixmap(16,16);
    pixmap.fill( Qt::white );
    pixmap.setMask(pixmap.createMaskFromColor(Qt::white));
    return pixmap;
}


APEButtonPixmapGenerator::APEButtonPixmapGenerator(
    QStyle::PrimitiveElement id,
    QStyle::SubElement sub,
    QStyle::StateFlag options):
	APixmapGenerator(APixmapGenerator::Generated),
	id_(id),
	sub_(sub),
	options_(options)
{}

QPixmap APEButtonPixmapGenerator::operator()()
{
    QStyleOptionButton b;
    QRect r = QApplication::style()->subElementRect(sub_,&b);
    QPixmap pixmap(r.width(),r.height());
    QPainter paint(&pixmap);
    b.state |= options_;
    b.rect = QRect(0,0,r.width(),r.height());
    QApplication::style()->drawPrimitive(id_,&b,&paint);
    return pixmap;
}

AFilePixmapGenerator::AFilePixmapGenerator(const QString& name):
    APixmapGenerator(APixmapGenerator::File),
    name_(name)
{}

QPixmap AFilePixmapGenerator::operator()()
{
    return QPixmap(images_path + name_);
}

	
void initPixmaps()
{
	pix_map["theme:information"] = new AStdPixmapGenerator(QStyle::SP_MessageBoxInformation);
	pix_map["theme:warning"] = new AStdPixmapGenerator(QStyle::SP_MessageBoxWarning);
	pix_map["theme:critical"] = new AStdPixmapGenerator(QStyle::SP_MessageBoxCritical);
	pix_map["theme:question"] = new AStdPixmapGenerator(QStyle::SP_MessageBoxQuestion);

	pix_map["theme:desktop"] = new AStdPixmapGenerator(QStyle::SP_DesktopIcon);
	pix_map["theme:trash"] = new AStdPixmapGenerator(QStyle::SP_TrashIcon);
	pix_map["theme:computer"] = new AStdPixmapGenerator(QStyle::SP_ComputerIcon);

	pix_map["theme:disk-fd"] = new AStdPixmapGenerator(QStyle::SP_DriveFDIcon);
	pix_map["theme:disk-hd"] = new AStdPixmapGenerator(QStyle::SP_DriveHDIcon);
	pix_map["theme:disk-cd"] = new AStdPixmapGenerator(QStyle::SP_DriveCDIcon);
	pix_map["theme:disk-dvd"] = new AStdPixmapGenerator(QStyle::SP_DriveDVDIcon);
	pix_map["theme:disk-net"] = new AStdPixmapGenerator(QStyle::SP_DriveNetIcon);
	pix_map["theme:directory"] = new AStdPixmapGenerator(QStyle::SP_DirOpenIcon);
	pix_map["theme:file"] = new AStdPixmapGenerator(QStyle::SP_FileIcon);

	pix_map["theme:unknown"] = new AStdPixmapGenerator(QStyle::SP_TitleBarMenuButton);

    	pix_map["theme:check-on"] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorCheckBox,
	                                          QStyle::SE_CheckBoxIndicator,
						  QStyle::State_On);
    	pix_map["theme:check-off"] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorCheckBox,
	                                           QStyle::SE_CheckBoxIndicator,
						   QStyle::State_Off);
    	pix_map["theme:radio-on"] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorRadioButton,
	                                          QStyle::SE_RadioButtonIndicator,
						  QStyle::State_On);
    	pix_map["theme:radio-off"] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorRadioButton,
	                                           QStyle::SE_RadioButtonIndicator,
						   QStyle::State_Off);

    	pix_map["theme:null"] = new ANullPixmapGenerator();
	pix_map["theme:help"] = new AFilePixmapGenerator("help.png");

    pix_map_initialised = true;
}

QPixmap getPixmap(QString id)
{
    if( !pix_map_initialised )
	initPixmaps();

    if( id.isEmpty() )
	id = "theme:null";
    
    QPixmap pixmap;
    if( !QPixmapCache::find(id, pixmap) )
    {
	APixmapGenerator *pixgen = 0;
	if( id.startsWith("theme:") && pix_map.contains(id) )
	    pixgen = pix_map[id];

	if( pixgen )
    	    pixmap = (*pixgen)();
	else
    	    pixmap = QPixmap(images_path + id);

	if( pixmap.isNull() )
	    pixmap = (*pix_map["theme:unknown"])();
	else if( (pixgen && pixgen->type() == APixmapGenerator::Generated )
		|| (pixmap.width() < 48 && pixmap.height() < 48) )
    	    QPixmapCache::insert(id, pixmap);
    }
    return pixmap;
}

APixmaps::APixmaps()
{
    standard["theme:information"] = QStyle::SP_MessageBoxInformation;
    standard["theme:warning"] = QStyle::SP_MessageBoxWarning;
    standard["theme:critical"] = QStyle::SP_MessageBoxCritical;
    standard["theme:question"] = QStyle::SP_MessageBoxQuestion;

    standard["theme:desktop"] = QStyle::SP_DesktopIcon;
    standard["theme:trash"] = QStyle::SP_TrashIcon;
    standard["theme:computer"] = QStyle::SP_ComputerIcon;

    standard["theme:disk-fd"] = QStyle::SP_DriveFDIcon;
    standard["theme:disk-hd"] = QStyle::SP_DriveHDIcon;
    standard["theme:disk-cd"] = QStyle::SP_DriveCDIcon;
    standard["theme:disk-dvd"] = QStyle::SP_DriveDVDIcon;
    standard["theme:disk-net"] = QStyle::SP_DriveNetIcon;
    standard["theme:directory"] = QStyle::SP_DirOpenIcon;
    standard["theme:file"] = QStyle::SP_FileIcon;

    standard["theme:unknown"] = QStyle::SP_TitleBarMenuButton;

    generated["theme:check-on"] = qMakePair(QStyle::PE_IndicatorCheckBox, (int)QStyle::State_On);
    generated["theme:check-off"] = qMakePair(QStyle::PE_IndicatorCheckBox, (int)QStyle::State_Off);
    generated["theme:radio-on"] = qMakePair(QStyle::PE_IndicatorRadioButton, (int)QStyle::State_On);
    generated["theme:radio-off"] = qMakePair(QStyle::PE_IndicatorRadioButton, (int)QStyle::State_Off);

    generated["theme:null"] = qMakePair((QStyle::PrimitiveElement)0, 0);

    filesystem["theme:help"] = "help.png";
}

APixmaps::~APixmaps() {}

QPixmap APixmaps::get(const QString &x_id)
{
    QString id = x_id;
    QPixmap pixmap;

    if( id.isEmpty() )
    {
	id = "theme:null";
    }

    if( !QPixmapCache::find(id, pixmap) )
    {
	bool is_generated = false;
	if( id.startsWith("theme:") )
	{
	    if( standard.contains(id) )
	    {
		//qDebug("selected standard pixmap");
		pixmap = QApplication::style()->standardPixmap(standard[id]);
	    }
	    else if( generated.contains(id) )
	    {
		//qDebug("selected generated pixmap");
		is_generated = true;
		pixmap = generate(id);
	    }
	    else if( filesystem.contains(id) )
	    {
		// FIXME may be images path changeble ?
		pixmap = QPixmap( IMAGES_PATH + filesystem[id] );
	    }
	}
	else
	    pixmap = QPixmap( IMAGES_PATH + id );

	if( pixmap.isNull() )
	    pixmap = QApplication::style()->standardPixmap(standard["theme:unknown"]);
	if( is_generated || (pixmap.width() < 48 && pixmap.height() < 48) )
	    QPixmapCache::insert(id, pixmap);
    }
    return pixmap;
}

QPixmap APixmaps::generate(const QString &id)
{
    QPixmap pixmap;
    if( generated.contains(id) )
    {
	QPair<QStyle::PrimitiveElement, int> pair(generated[id]);
	switch( pair.first )
	{
	    case 0:
	    {
		pixmap = QPixmap(16,16);
		pixmap.fill( Qt::white );
		pixmap.setMask(pixmap.createMaskFromColor(Qt::white));
		break;
	    }
	    case QStyle::PE_IndicatorCheckBox:
	    {
		QStyle::StateFlag opt = (QStyle::StateFlag)pair.second;
    		QStyleOptionButton b;
		QRect r = QApplication::style()->subElementRect(QStyle::SE_CheckBoxIndicator,&b);
		pixmap = QPixmap(r.width(),r.height());
		QPainter paint(&pixmap);
		b.state |= opt;
		b.rect = QRect(0,0,r.width(),r.height());
		QApplication::style()->drawPrimitive(pair.first,&b,&paint);
		break;
	    }
	    case QStyle::PE_IndicatorRadioButton:
	    {
		QStyle::StateFlag opt = (QStyle::StateFlag)pair.second;
    		QStyleOptionButton b;
		QRect r = QApplication::style()->subElementRect(QStyle::SE_RadioButtonIndicator,&b);
		pixmap = QPixmap(r.width(),r.height());
		QPainter paint(&pixmap);
		b.state |= opt;
		b.rect = QRect(0,0,r.width(),r.height());
		QApplication::style()->drawPrimitive(pair.first,&b,&paint);
		break;
	    }
	    default:
		pixmap = QApplication::style()->standardPixmap(standard["theme:unknown"]);
		break;
	}
    }
    return pixmap;
}
