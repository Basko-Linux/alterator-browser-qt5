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
	return QApplication::style()->standardIcon(id_).pixmap(22,22);
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
    return QPixmap(QStringLiteral(":/images/altlinux_16.png"));
}

QPixmap APEButtonPixmapGenerator::operator()()
{
    QStyleOptionButton b;
    QRect r = QApplication::style()->subElementRect(sub_,&b);
    QPixmap pixmap(r.width(),r.height());
    //QColor bgcolor =  QGuiApplication::palette().color(QPalette::Active, QPalette::Window);
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
	if(px.isNull()) {
	    Q_FOREACH(QString ext, QStringList() << QStringLiteral(".png") << QStringLiteral(".svg") << QStringLiteral(".jpg") << QString() ) {
		pixpath = images_path + name_ + ext;
		if( QFile::exists(pixpath) ) {
		    px = QPixmap(pixpath);
		}
		if(!px.isNull()) {
		    break;
		}
	    }
	} else {
	    break;
	}
    }
#if 0
    if(px.isNull()) {
	px = QIcon::fromTheme(name_).pixmap(22,22);
    }
#endif
    return px;
}
	
void initPixmaps()
{
	pix_map[QStringLiteral("theme:information")] = new AStdPixmapGenerator(QStyle::SP_MessageBoxInformation);
	pix_map[QStringLiteral("theme:warning")] = new AStdPixmapGenerator(QStyle::SP_MessageBoxWarning);
	pix_map[QStringLiteral("theme:critical")] = new AStdPixmapGenerator(QStyle::SP_MessageBoxCritical);
	pix_map[QStringLiteral("theme:question")] = new AStdPixmapGenerator(QStyle::SP_MessageBoxQuestion);

	pix_map[QStringLiteral("theme:yes")] = new AStdPixmapGenerator(QStyle::SP_DialogYesButton);
	pix_map[QStringLiteral("theme:no")] = new AStdPixmapGenerator(QStyle::SP_DialogNoButton);
	pix_map[QStringLiteral("theme:ok")] = new AStdPixmapGenerator(QStyle::SP_DialogOkButton);
	pix_map[QStringLiteral("theme:apply")] = new AStdPixmapGenerator(QStyle::SP_DialogApplyButton);
	pix_map[QStringLiteral("theme:cancel")] = new AStdPixmapGenerator(QStyle::SP_DialogCancelButton);

	pix_map[QStringLiteral("theme:desktop")] = new AStdPixmapGenerator(QStyle::SP_DesktopIcon);
	pix_map[QStringLiteral("theme:trash")] = new AStdPixmapGenerator(QStyle::SP_TrashIcon);
	pix_map[QStringLiteral("theme:computer")] = new AStdPixmapGenerator(QStyle::SP_ComputerIcon);

	pix_map[QStringLiteral("theme:forward")] = new AStdPixmapGenerator(QStyle::SP_ArrowForward);
	pix_map[QStringLiteral("theme:backward")] = new AStdPixmapGenerator(QStyle::SP_ArrowBack);
	pix_map[QStringLiteral("theme:up")] = new AStdPixmapGenerator(QStyle::SP_ArrowUp);
	pix_map[QStringLiteral("theme:down")] = new AStdPixmapGenerator(QStyle:: SP_ArrowDown);
	pix_map[QStringLiteral("theme:left")] = new AStdPixmapGenerator(QStyle::SP_ArrowLeft);
	pix_map[QStringLiteral("theme:right")] = new AStdPixmapGenerator(QStyle::SP_ArrowRight);

	pix_map[QStringLiteral("theme:disk-fd")] = new AStdPixmapGenerator(QStyle::SP_DriveFDIcon);
	pix_map[QStringLiteral("theme:disk-hd")] = new AStdPixmapGenerator(QStyle::SP_DriveHDIcon);
	pix_map[QStringLiteral("theme:disk-cd")] = new AStdPixmapGenerator(QStyle::SP_DriveCDIcon);
	pix_map[QStringLiteral("theme:disk-dvd")] = new AStdPixmapGenerator(QStyle::SP_DriveDVDIcon);
	pix_map[QStringLiteral("theme:disk-net")] = new AStdPixmapGenerator(QStyle::SP_DriveNetIcon);
	pix_map[QStringLiteral("theme:directory")] = new AStdPixmapGenerator(QStyle::SP_DirOpenIcon);
	pix_map[QStringLiteral("theme:file")] = new AStdPixmapGenerator(QStyle::SP_FileIcon);

	pix_map[QStringLiteral("theme:unknown")] = new AUnknownPixmapGenerator();

    	pix_map[QStringLiteral("theme:check-on")] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorCheckBox,
	                                          QStyle::SE_CheckBoxIndicator,
						  QStyle::State_On|QStyle::State_Enabled);
    	pix_map[QStringLiteral("theme:check-off")] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorCheckBox,
	                                           QStyle::SE_CheckBoxIndicator,
						   QStyle::State_Off|QStyle::State_Enabled);
    	pix_map[QStringLiteral("theme:radio-on")] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorRadioButton,
	                                          QStyle::SE_RadioButtonIndicator,
						  QStyle::State_On|QStyle::State_Enabled);
    	pix_map[QStringLiteral("theme:radio-off")] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorRadioButton,
	                                           QStyle::SE_RadioButtonIndicator,
						   QStyle::State_Off|QStyle::State_Enabled);

    	pix_map[QStringLiteral("theme:arrow-up")] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorArrowUp,
	                                           QStyle::SE_RadioButtonIndicator,
						   QStyle::State_Off|QStyle::State_Enabled);
    	pix_map[QStringLiteral("theme:arrow-down")] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorArrowDown,
	                                           QStyle::SE_RadioButtonIndicator,
						   QStyle::State_Off|QStyle::State_Enabled);
    	pix_map[QStringLiteral("theme:arrow-right")] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorArrowRight,
	                                           QStyle::SE_RadioButtonIndicator,
						   QStyle::State_Off|QStyle::State_Enabled);
    	pix_map[QStringLiteral("theme:arrow-left")] = new APEButtonPixmapGenerator(QStyle::PE_IndicatorArrowLeft,
	                                           QStyle::SE_RadioButtonIndicator,
						   QStyle::State_Off|QStyle::State_Enabled);

    	pix_map[QStringLiteral("theme:null")] = new ANullPixmapGenerator();
	pix_map[QStringLiteral("theme:help")] = new AStdPixmapGenerator(QStyle::SP_DialogHelpButton);
	pix_map[QStringLiteral("theme:close")] = new AStdPixmapGenerator(QStyle::SP_DialogCloseButton);

    pix_map_initialised = true;
}

QPixmap getPixmap(QString id)
{
    if( !pix_map_initialised )
	initPixmaps();

    if( id.isEmpty() )
	id = QStringLiteral("theme:null");
    
    QPixmap pixmap;
    if( !QPixmapCache::find(id, &pixmap) )
    {
	APixmapGenerator *pixgen = 0;
	if( id.startsWith(QStringLiteral("theme:")) )
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
	    pixmap = (*pix_map[QStringLiteral("theme:unknown")])();
	else if( (pixgen && pixgen->type() == APixmapGenerator::Generated )
		|| (pixmap.width() < 48 && pixmap.height() < 48) )
    	    QPixmapCache::insert(id, pixmap);
    }
    return pixmap;
}
