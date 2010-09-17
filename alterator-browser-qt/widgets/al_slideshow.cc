
#include <QDir>

#include "al_slideshow.hh"

SlideLoader::SlideLoader(QWidget *parent):
    QThread(parent)
{
    parent_ = parent;
    current_img_ = 0;
    stop_ = false;

    tmr = new QTimer(this);
    tmr->setSingleShot(true);
    tmr->setInterval(8000);
    connect(tmr, SIGNAL(timeout()), this, SLOT(quit()));
}

SlideLoader::~SlideLoader()
{
    stop();
}

void SlideLoader::stop()
{
    stop_ = true;
    quit();
    wait();
}

void SlideLoader::setSource(const QString &new_src_dir)
{
    src_dir_ = new_src_dir;
    images_.clear();
    QDir imgdir(new_src_dir);
    if( imgdir.exists() )
    {
	QStringList name_filters;
	name_filters << "*.jpg" << "*.png" << "*.gif";
	images_ = imgdir.entryList(name_filters, QDir::Files|QDir::NoDotAndDotDot|QDir::Readable, QDir::Name);
	if( current_img_ )
	    delete current_img_;
	current_img_ = new QStringListIterator(images_);
	current_img_->toFront();
    }
    stop_ = true;
    while( !wait() ) {}
    if( images_.size() > 0 )
    {
	stop_ = false;
	start(QThread::LowPriority);
    }
}

void SlideLoader::setInterval(int new_interval)
{
    if( new_interval == 0 )
    {
	stop_ = true;
    }
    else if( new_interval > 0 )
    {
	tmr->setInterval(new_interval);
    }
}

QImage SlideLoader::image()
{
    current_image_lock_.lock();
    QImage ret(current_image_);
    current_image_lock_.unlock();
    return ret;
}

void SlideLoader::run()
{
    while( !stop_ )
    {
	if( current_img_ )
	{
	    if( !current_img_->hasNext() )
		current_img_->toFront();
	    QString imgfile = src_dir_ + QDir::separator() + current_img_->next();
	    {
		QImage image(imgfile);
		if( !image.isNull() )
		{
		    if( (image.width() > parent_->width() || image.height() > parent_->height()) && !stop_ )
			image = image.scaled(parent_->width(), parent_->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		    if( !stop_ )
		    {
			current_image_lock_.lock();
			current_image_ = image;
			current_image_lock_.unlock();
		    }
		    if( !stop_ )
			emit gotImage();
		}
		else
		    qDebug("Unable to load picture: %s", qPrintable(imgfile));
	    }
	}
	if( stop_ )
	{
	    stop_ = false;
	    break;
	}
	else
	{
	    tmr->start();
	    exec();
	}
    }
}


// ASlideShow
ASlideShow::ASlideShow(QWidget *parent, const Qt::Orientation):
    AWidget<QLabel>(parent)
{
    setWordWrap( true );
    setAlignment( Qt::AlignCenter );

    loader = new SlideLoader(this);
    connect(loader, SIGNAL(gotImage()), this, SLOT(setPixmap()));
}

ASlideShow::~ASlideShow()
{
    loader->stop();
}

void ASlideShow::setInterval(int step)
{
    loader->setInterval(step);
}

void ASlideShow::setSource(const QString& new_src)
{
    loader->setSource(new_src);
}

void ASlideShow::stop()
{
    loader->stop();
}

void ASlideShow::setPixmap()
{
    QLabel::setPixmap(QPixmap::fromImage(loader->image()));
}

// alSlideShow
alSlideShow::alSlideShow(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent):
	alWidgetPre<ASlideShow>(attr,WSlideShow,id,parent)
{
}

alSlideShow::~alSlideShow()
{
    wnd_->stop();
}

void alSlideShow::setAttr(const QString& name,const QString& value)
{
    if( "url" == name || "text" == name )
        wnd_->setSource(value);
    else if( "start" == name )
	wnd_->setInterval(-1);
    else if( "stop" == name )
	wnd_->setInterval(0);
    else if( "step" == name )
    {
	bool iok;
	int step = value.toInt(&iok);
	if(iok)
	    wnd_->setInterval(step);
    }
    else
        alWidget::setAttr(name,value);
}
