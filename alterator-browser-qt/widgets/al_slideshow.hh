#ifndef QTBROWSER_AL_SLIDESHOW_HH
#define QTBROWSER_AL_SLIDESHOW_HH

#include <QLabel>
#include <QPixmap>
#include <QMovie>

#include "al_widget.hh"

class SlideLoader: public QThread
{
Q_OBJECT
public:
    SlideLoader(QWidget*);
    ~SlideLoader();

    void run();
    void setSource(const QString&);
    void setInterval(int);
    QImage image();

signals:
    void gotImage();

private:
    QWidget *parent_;
    QString src_dir_;
    QStringList images_;
    QStringListIterator *current_img_;
    QTimer *tm_;
    int interval_;
    bool stop_;
    QImage current_image_;
};

class ASlideShow: public AWidget<QLabel>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ASlideShow(QWidget *parent, const Qt::Orientation);
    ~ASlideShow();

    void setSource(const QString&);
    void setInterval(int);

public slots:
    void setPixmap();

private:
    SlideLoader *loader;
};

class alSlideShow: public alWidgetPre<ASlideShow>
{
Q_OBJECT
public:
    alSlideShow(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent);
    void setAttr(const QString& name,const QString& value);
};

#endif
