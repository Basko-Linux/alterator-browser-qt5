
#include <QDir>

#include "al_slideshow.hh"

#define SLIDESHOW_INTERVAL_DEF 8000
#define SLIDESHOW_INTERVAL_MIN 1000

// ASlideShow
ASlideShow::ASlideShow(QWidget *parent, const Qt::Orientation):
    AWidget<QLabel>(parent)
{
    current_img_ = 0;
    m_continue = false;
    m_once = false;

    setAlignment( Qt::AlignCenter );

    next_slide_tmr = new QTimer(this);
    next_slide_tmr->setSingleShot(true);
    next_slide_tmr->setInterval(SLIDESHOW_INTERVAL_DEF);

    connect(next_slide_tmr, SIGNAL(timeout()), this, SLOT(applyPixmap()));
}

ASlideShow::~ASlideShow()
{
}

void ASlideShow::setInterval(int new_interval)
{
    if( new_interval * 1000 > SLIDESHOW_INTERVAL_MIN )
	next_slide_tmr->setInterval(new_interval * 1000);
}

void ASlideShow::start()
{
    m_continue = true;
    next_slide_tmr->start();
}

void ASlideShow::stop()
{
    m_continue = false;
    next_slide_tmr->stop();
}

void ASlideShow::next()
{
    applyPixmap();
}

void ASlideShow::prev()
{
	if( current_img_ )
	{
	    if( current_img_->hasPrevious() )
		current_img_->previous();
	    if( current_img_->hasPrevious() )
		current_img_->previous();
	    else
		current_img_->toFront();
	    applyPixmap();
	}
}

void ASlideShow::setOnce(bool want_once)
{
    m_once = want_once;
}

void ASlideShow::applyPixmap()
{
	if( current_img_ )
	{
	    if( !current_img_->hasNext() )
	    {
		if( m_once )
		{
		    stop();
		    return;
		}
		current_img_->toFront();
	    }
	    QString imgfile = src_dir_ + QDir::separator() + current_img_->next();

	    if( movie() )
		delete movie();

		    if( imgfile.endsWith(QStringLiteral(".svg")) || imgfile.endsWith(QStringLiteral(".jpg")) || imgfile.endsWith(QStringLiteral(".png")) )
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
		    else if( imgfile.endsWith(QStringLiteral(".gif")) || imgfile.endsWith(QStringLiteral(".mng")) )
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
		
		    if( m_continue ) start();
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
	name_filters << QStringLiteral("*.svg") << QStringLiteral("*.jpg") << QStringLiteral("*.png") << QStringLiteral("*.gif") << QStringLiteral("*.mng");
	images_ = imgdir.entryList(name_filters, QDir::Files|QDir::NoDotAndDotDot|QDir::Readable, QDir::Name);
	if( current_img_ )
	{
	    delete current_img_;
	    current_img_ = 0;
	}
	if( images_.size() > 0 )
	{
	    current_img_ = new QStringListIterator(images_);
	    current_img_->toFront();
	}
    }

    if( images_.size() > 0 )
	QTimer::singleShot(0, this, SLOT(applyPixmap()));
    else
	stop();
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
    if( QStringLiteral("url") == name || QStringLiteral("text") == name )
        wnd_->setSource(value);
    if( QStringLiteral("once") == name )
        wnd_->setOnce( QStringLiteral("false") != value );
    else if( QStringLiteral("start") == name )
	wnd_->start();
    else if( QStringLiteral("stop") == name )
	wnd_->stop();
    else if( QStringLiteral("next") == name )
	wnd_->next();
    else if( QStringLiteral("prev") == name )
	wnd_->prev();
    else if( QStringLiteral("step") == name )
    {
	bool iok;
	int step = value.toInt(&iok);
	if(iok)
	    wnd_->setInterval(step);
    }
    else
        alWidget::setAttr(name,value);
}
