
#include <QTimer>
#include <QDir>

#include "al_slideshow.hh"

SlideLoader::SlideLoader(QWidget *parent):
    QThread(parent)
{
    interval_ = 8000;
    parent_ = parent;
    current_img_ = 0;
    tm_ = new QTimer(this);
    stop_ = false;

    connect(tm_, SIGNAL(timeout()), this, SLOT(quit()));
}

SlideLoader::~SlideLoader()
{
    tm_->stop();
    stop_ = true;
    quit();
}

void SlideLoader::setSource(const QString &new_src_dir)
{
    src_dir_ = new_src_dir;
    tm_->stop();
    QDir imgdir(new_src_dir);
    if( imgdir.exists() )
    {
	QStringList name_filters;
	name_filters << "*.jpg" << "*.png" << "*.gif" << "*.mng";
	images_ = imgdir.entryList(name_filters, QDir::Files|QDir::NoDotAndDotDot|QDir::Readable, QDir::Name);
	if( current_img_ )
	{
	    QStringListIterator *current_img_old = current_img_;
	    current_img_ = new QStringListIterator(images_);
	    current_img_->toFront();
	    delete current_img_old;
	    current_img_old = 0;
	}
    }
    stop_ = true;
    while( !wait() ) {}
    stop_ = false;
    start();
    tm_->start(interval_);
}

void SlideLoader::setInterval(int new_interval)
{
    if( new_interval == 0 )
    {
	tm_->stop();
    }
    else
    {
	if(new_interval >= 1000)
	    interval_ = new_interval;
	tm_->start(interval_);
    }
}

void SlideLoader::run()
{
    while( !stop_ )
    {
	tm_->stop();
	if( current_img_ )
	{
	    if( !current_img_->hasNext() )
		current_img_->toFront();
	    QString imgfile = src_dir_ + QDir::separator() + current_img_->next();
	    if( imgfile.endsWith("*.gif") || imgfile.endsWith("*.mng") )
	    {
		QMovie movie(imgfile);
		if( movie.isValid() )
		    emit gotMovie(movie);
		else
		    qDebug("Unable to load movie: %s", qPrintable(imgfile));
	    }
	    else
	    {
		QPixmap pixmap(imgfile);
		if( !pixmap.isNull() )
		{
		    if( pixmap.width() > parent_->width() || pixmap.height() > parent_->height() )
			pixmap = pixmap.scaled(parent_->width(), parent_->height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
		    emit gotPixmap(pixmap);
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
	    tm_->start(interval_);
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
    connect(loader, SIGNAL(gotPixmap(const QPixmap&)), this, SLOT(setPixmap(const QPixmap&)));
    connect(loader, SIGNAL(gotMovie(const QMovie&)), this, SLOT(setMovie(const QMovie&)));
}

ASlideShow::~ASlideShow() {}

void ASlideShow::setInterval(int step)
{
    loader->setInterval(step);
}

void ASlideShow::setSource(const QString& new_src)
{
    loader->setSource(new_src);
}

alSlideShow::alSlideShow(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent):
	alWidgetPre<ASlideShow>(attr,WSlideShow,id,parent)
{
}

// alSlideShow
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
