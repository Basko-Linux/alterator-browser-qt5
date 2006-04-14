#include <QPixmapCache>
#include <QApplication>
#include <QPainter>
#include <QRect>
#include <QStyleOptionButton>
#include <QBitmap>

#include "a_pixmaps.hh"

#define IMAGES_PATH "/usr/share/alterator/images/"

QString images_path(IMAGES_PATH);
pix_map_t pix_map;
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
