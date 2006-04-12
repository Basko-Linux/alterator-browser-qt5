#include <QPixmapCache>
#include <QApplication>
#include <QPainter>
#include <QRect>
#include <QStyleOptionButton>

#include "a_pixmaps.hh"

#define IMAGES_PATH "/usr/share/alterator/images/"

APixmaps::APixmaps()
{
    standard["theme:information"] = QStyle::SP_MessageBoxInformation;
    standard["theme:warning"] = QStyle::SP_MessageBoxWarning;
    standard["theme:critical"] = QStyle::SP_MessageBoxCritical;
    standard["theme:question"] = QStyle::SP_MessageBoxQuestion;

    standard["theme:desktop"] = QStyle::SP_DesktopIcon;
    standard["theme:trash"] = QStyle::SP_TrashIcon;
    standard["theme:computer"] = QStyle::SP_ComputerIcon;

    standard["theme:fd"] = QStyle::SP_DriveFDIcon;
    standard["theme:hd"] = QStyle::SP_DriveHDIcon;
    standard["theme:cd"] = QStyle::SP_DriveCDIcon;
    standard["theme:dvd"] = QStyle::SP_DriveDVDIcon;
    standard["theme:netdisk"] = QStyle::SP_DriveNetIcon;
    standard["theme:dir"] = QStyle::SP_DirOpenIcon;
    standard["theme:file"] = QStyle::SP_FileIcon;

    standard["theme:unknown"] = QStyle::SP_TitleBarMenuButton;

    generated["theme:checkbox-on"] = qMakePair(QStyle::PE_IndicatorCheckBox, (int)QStyle::State_On);
    generated["theme:checkbox-off"] = qMakePair(QStyle::PE_IndicatorCheckBox, (int)QStyle::State_Off);
    generated["theme:radio-on"] = qMakePair(QStyle::PE_IndicatorRadioButton, (int)QStyle::State_On);
    generated["theme:radio-off"] = qMakePair(QStyle::PE_IndicatorRadioButton, (int)QStyle::State_Off);
}

APixmaps::~APixmaps() {}

QPixmap APixmaps::get(const QString &id)
{
    QPixmap pixmap;
    if( !id.isEmpty() && !QPixmapCache::find(id, pixmap) )
    {
	bool is_generated = false;
	if( id.startsWith("theme:") )
	{
	    if( standard.contains(id) )
	    {
		qDebug("selected standard pixmap");
		pixmap = QApplication::style()->standardPixmap(standard[id]);
	    }
	    else if( generated.contains(id) )
	    {
		qDebug("selected generated pixmap");
		is_generated = true;
		pixmap = generate(id);
	    }
	}
	else
	    pixmap = QPixmap( IMAGES_PATH + id );

	if( pixmap.width() <= 0 || pixmap.height() <= 0 )
	    pixmap = QApplication::style()->standardPixmap(standard["theme:unknown"]);
	else if( is_generated || (pixmap.width() <= 48 && pixmap.height() <= 48) )
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
