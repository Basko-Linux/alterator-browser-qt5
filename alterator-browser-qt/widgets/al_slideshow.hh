#ifndef QTBROWSER_AL_SLIDESHOW_HH
#define QTBROWSER_AL_SLIDESHOW_HH

#include <QLabel>
#include <QPixmap>
#include <QMovie>
#include <QMutex>

#include "al_widget.hh"

class SlideLoader: public QThread
{
Q_OBJECT
public:
    SlideLoader(QWidget*);
    ~SlideLoader();

    void run();
    void setSource(const QString&); // start
    void stop();
    void setInterval(int);
    QImage image();

Q_SIGNALS:
    void gotImage();

private:
    QWidget *parent_;
    QString src_dir_;
    QStringList images_;
    QStringListIterator *current_img_;
    bool stop_;
    QImage current_image_;
    QMutex current_image_lock_;
    QTimer *tmr;
};

class ASlideShow: public AWidget<QLabel>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ASlideShow(QWidget *parent, const Qt::Orientation);
    ~ASlideShow();

    void setSource(const QString&); // start
    void stop();
    void setInterval(int);

public Q_SLOTS:
    void setPixmap();

private:
    SlideLoader *loader;
};

class alSlideShow: public alWidgetPre<ASlideShow>
{
Q_OBJECT
public:
    alSlideShow(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent);
    ~alSlideShow();
    void setAttr(const QString& name,const QString& value);
};

#endif
