#ifndef QTBROWSER_AL_SLIDESHOW_HH
#define QTBROWSER_AL_SLIDESHOW_HH

#include <QLabel>
#include <QPixmap>
#include <QMovie>
#include <QMutex>

#include "al_widget.hh"

class ASlideShow: public AWidget<QLabel>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ASlideShow(QWidget *parent, const Qt::Orientation);
    ~ASlideShow();

    void setSource(const QString&); // start
    void start();
    void stop();
    void setInterval(int);
    void setOnce(bool);

public Q_SLOTS:
    void applyPixmap();

private:
    QTimer *next_slide_tmr;
    QString src_dir_;
    QStringList images_;
    QStringListIterator *current_img_;
    QImage current_image_;
    bool m_continue;
    bool m_once;
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
