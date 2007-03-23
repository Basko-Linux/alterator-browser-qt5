
#include <QTimer>
#include <QDir>

#include "al_slideshow.hh"

ASlideShow::ASlideShow(QWidget *parent):
    QLabel(parent)
{
    setWordWrap( true );
    setAlignment( Qt::AlignCenter );
    current_img = 0;
    tm = new QTimer(this);
    connect(tm, SIGNAL(timeout()), this, SLOT(showNextSlide()));
}

ASlideShow::~ASlideShow() {}

void ASlideShow::setSource(const QString& new_src)
{
    src = new_src;
    tm->stop();
    images.clear();
    delete current_img;
    current_img = 0;
    QDir imgdir(new_src);
    if( imgdir.exists() )
    {
	QStringList name_filters;
	name_filters << "*.jpg" << "*.png";
	images = imgdir.entryList(name_filters, QDir::Files|QDir::NoDotAndDotDot|QDir::Readable, QDir::Name);
	current_img = new QStringListIterator(images);
	current_img->toFront();
    }
    int n_images = images.count();
    if( n_images > 1 )
    {
	showNextSlide();
	tm->setInterval(8000);
	tm->start();
    }
    else if( n_images == 1 )
	showNextSlide();
    else
	clear();
}

void ASlideShow::showNextSlide()
{
    if( !current_img ) return;
    if( !current_img->hasNext() )
	current_img->toFront();
    QString imgfile = src + QDir::separator() + current_img->next();
    QPixmap pix = QPixmap(imgfile);
    if( !pix.isNull() )
    {
	if( pix.width() >= width() || pix.height() >= height() )
	    pix = pix.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
	setPixmap(pix);
    }
    else
	setText("Unable to load file: " + imgfile);
}

alSlideShow::alSlideShow(const QString& id, const QString& parent):
	alWidgetPre<ASlideShow>(SlideShow,id,parent)
{
}

void alSlideShow::setAttr(const QString& name,const QString& value)
{
    if( "text" == name )
        wnd_->setSource(value);
    else
        alWidget::setAttr(name,value);
}
