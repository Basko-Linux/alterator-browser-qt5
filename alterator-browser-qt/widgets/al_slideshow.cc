
#include <QDir>

#include "al_slideshow.hh"

// ASlideShow
ASlideShow::ASlideShow(QWidget *parent, const Qt::Orientation):
    AWidget<QLabel>(parent)
{
    current_img_ = 0;

    setAlignment( Qt::AlignCenter );

    next_slide_tmr = new QTimer(this);
    next_slide_tmr->setSingleShot(true);
    next_slide_tmr->setInterval(1000);

    connect(next_slide_tmr, SIGNAL(timeout()), this, SLOT(applyPixmap()));
}

ASlideShow::~ASlideShow()
{
}

void ASlideShow::setInterval(int new_interval)
{
    if( new_interval > 0 )
	next_slide_tmr->setInterval(new_interval);
    else
	next_slide_tmr->stop();
}

void ASlideShow::stop()
{
    next_slide_tmr->stop();
}

void ASlideShow::applyPixmap()
{
	if( current_img_ )
	{
	    if( !current_img_->hasNext() )
		current_img_->toFront();
	    QString imgfile = src_dir_ + QDir::separator() + current_img_->next();

	    if( movie() )
		delete movie();
//		movie()->deleteLater();

		    if( imgfile.endsWith(".jpg") || imgfile.endsWith(".png") )
		    {
			QPixmap pix(imgfile);
			if( !pix.isNull() )
			{
			    if( pix.width() > width() || pix.height() > height() )
			        pix = pix.scaled(width(), height(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
			    setPixmap(pix);
			}
			else
			    qDebug("Unable to load picture: %s", qPrintable(imgfile));
		    }
		    else if( imgfile.endsWith(".gif") || imgfile.endsWith(".mng") )
		    {
			QMovie *anim = new QMovie(imgfile, QByteArray(), this);
			if( anim->isValid() )
			{
			    setMovie(anim);
			    anim->start();
			}
			else
			    qDebug("Unable to load animation: %s", qPrintable(imgfile));
		    }
		    else
			qDebug("Unknown file: %s", qPrintable(imgfile));
		    next_slide_tmr->start();
	}
}

void ASlideShow::setSource(const QString &new_src_dir)
{
    src_dir_ = new_src_dir;
    images_.clear();
    QDir imgdir(new_src_dir);
    if( imgdir.exists() )
    {
	QStringList name_filters;
	name_filters << "*.jpg" << "*.png" << "*.gif" << "*.mng";
	images_ = imgdir.entryList(name_filters, QDir::Files|QDir::NoDotAndDotDot|QDir::Readable, QDir::Name);
	if( current_img_ )
	    delete current_img_;
	current_img_ = new QStringListIterator(images_);
	current_img_->toFront();
    }
    if( images_.size() > 0 )
    {
	//next_slide_tmr->start();
	QTimer::singleShot(0, this, SLOT(applyPixmap()));
    }
    else
	next_slide_tmr->stop();
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
