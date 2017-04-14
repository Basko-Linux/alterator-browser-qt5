#include <QPixmapCache>
#include <QApplication>
#include <QPainter>
#include <QRect>
#include <QStyleOptionButton>
#include <QBitmap>
#include <QFile>

#include "a_pixmaps.hh"

QStringList pixmap_paths;

pix_map_t pix_map;
bool pix_map_initialised = false;

QPixmap AStdPixmapGenerator::operator()()
{
	return QApplication::style()->standardPixmap(id_);
}

QPixmap ANullPixmapGenerator::operator()()
{
    QPixmap pixmap(16,16);
    pixmap.fill( QColor(0,0,0,0) );
//    pixmap.fill( Qt::white );
//    pixmap.setMask(pixmap.createMaskFromColor(Qt::white));
    return pixmap;
}

QPixmap AUnknownPixmapGenerator::operator()()
{
    return QPixmap(":/images/altlinux_16.png");
}

QPixmap APEButtonPixmapGenerator::operator()()
{
    QStyleOptionButton b;
    QRect r = QApplication::style()->subElementRect(sub_,&b);
    QPixmap pixmap(r.width(),r.height());
    //QColor bgcolor =  QGuiApplication::palette().color(QPalette::Active, QPalette::Background);
    QColor bgcolor(0,0,0,0);
    pixmap.fill(bgcolor);
    QPainter paint(&pixmap);
    b.state |= (QStyle::State)options_;
    b.rect = QRect(0,0,r.width(),r.height());
    QApplication::style()->drawPrimitive(id_,&b,&paint);
    //pixmap.setMask(pixmap.createMaskFromColor(bgcolor));
    return pixmap;
}

QPixmap AFilePixmapGenerator::operator()()
{
    QPixmap px;
    Q_FOREACH(QString images_path, pixmap_paths)
    {
	QString pixpath;
	if(px.isNull())
	{
	    pixpath = images_path + name_ + ".png";
	    if( QFile::exists(pixpath) )
		px = QPixmap(pixpath);
	}
	else
	    break;
	if(px.isNull())
	{
	    pixpath = images_path + name_;
	    if( QFile::exists(pixpath) )
		px = QPixmap(images_path + name_);
	    if(!px.isNull())
		qDebug("Using pixmap name \"%s\" with file extension.", qPrintable(name_));
	}
	else
	    break;
    }
    return px;
}
	
void initPixmaps()
{
	pix_map["theme:information"] = new AStdPixmapGenerator(QStyle::SP_MessageBoxInformation);
	pix_map["theme:warning"] = new AStdPixmapGenerator(QStyle::SP_MessageBoxWarning);
	pix_map["theme:critical"] = new AStdPixmapGenerator(QStyle::SP_MessageBoxCritical);
	pix_map["theme:question"] = new AStdPixmapGenerator(QStyle::SP_MessageBoxQuestion);

	pix_map["theme:yes"] = new AStdPixmapGenerator(QStyle::SP_DialogYesButton);
	pix_map["theme:no"] = new AStdPixmapGenerator(QStyle::SP_DialogNoButton);
	pix_map["theme:ok"] = new AStdPixmapGenerator(QStyle::SP_DialogOkButton);
	pix_map["theme:apply"] = new AStdPixmapGenerator(QStyle::SP_DialogApplyButton);
	pix_map["theme:cancel"] = new AStdPixmapGenerator(QStyle::SP_DialogCancelButton);

	pix_map["theme:desktop"] = new AStdPixmapGenerator(QStyle::SP_DesktopIcon);
	pix_map["theme:trash"] = new AStdPixmapGenerator(QStyle::SP_TrashIcon);
	pix_map["theme:computer"] = new AStdPixmapGenerator(QStyle::SP_ComputerIcon);

	pix_map["theme:forward"] = new AStdPixmapGenerator(QStyle::SP_ArrowForward);
	pix_map["theme:backward"] = new AStdPixmapGenerator(QStyle::SP_ArrowBack);
	pix_map["theme:up"] = new AStdPixmapGenerator(QStyle::SP_ArrowUp);
	pix_map["theme:down"] = new AStdPixmapGenerator(QStyle:: SP_ArrowDown);
	pix_map["theme:left"] = new AStdPixmapGenerator(QStyle::SP_ArrowLeft);
	pix_map["theme:right"] = new AStdPixmapGenerator(QStyle::SP_ArrowRight);

	pix_map["theme:disk-fd"] = new AStdPixmapGenerator(QStyle::SP_DriveFDIcon);
	pix_map["theme:disk-hd"] = new AStdPixmapGenerator(QStyle::SP_DriveHDIcon);
	pix_map["theme:disk-cd"] = new AStdPixmapGenerator(QStyle::SP_DriveCDIcon);
	pix_map["theme:disk-dvd"] = new AStdPixmapGenerator(QStyle::SP_DriveDVDIcon);
	pix_map["theme:disk-net"] = new AStdPixmapGenerator(QStyle::SP_DriveNetIcon);
	pix_map["theme:directory"] = new AStdPixmapGenerator(QStyle::SP_DirOpenIcon);
	pix_map["theme:file"] = new AStdPixmapGenerator(QStyle::SP_FileIcon);

	pix_map["theme:unknown"] = new AUnknownPixmapGenerator();

    	pix_map["theme:check-on"] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorCheckBox,
	                                          QStyle::SE_CheckBoxIndicator,
						  QStyle::State_On|QStyle::State_Enabled);
    	pix_map["theme:check-off"] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorCheckBox,
	                                           QStyle::SE_CheckBoxIndicator,
						   QStyle::State_Off|QStyle::State_Enabled);
    	pix_map["theme:radio-on"] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorRadioButton,
	                                          QStyle::SE_RadioButtonIndicator,
						  QStyle::State_On|QStyle::State_Enabled);
    	pix_map["theme:radio-off"] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorRadioButton,
	                                           QStyle::SE_RadioButtonIndicator,
						   QStyle::State_Off|QStyle::State_Enabled);

    	pix_map["theme:null"] = new ANullPixmapGenerator();
	pix_map["theme:help"] = new AStdPixmapGenerator(QStyle::SP_DialogHelpButton);

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
	if( id.startsWith("theme:") )
	{
	    QString fid = id.mid(6);
	    pixmap = AFilePixmapGenerator(fid)();
	    if( pixmap.isNull() )
	    {
		if( pix_map.contains(id) )
		    pixgen = pix_map[id];
		if( pixgen )
    		    pixmap = (*pixgen)();
	    }
	}
	else
	    pixmap = AFilePixmapGenerator(id)();

	if( pixmap.isNull() )
	    pixmap = (*pix_map["theme:unknown"])();
	else if( (pixgen && pixgen->type() == APixmapGenerator::Generated )
		|| (pixmap.width() < 48 && pixmap.height() < 48) )
    	    QPixmapCache::insert(id, pixmap);
    }
    return pixmap;
}
