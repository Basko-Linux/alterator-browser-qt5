#ifndef QTBROWSER_AL_SLIDESHOW_HH
#define QTBROWSER_AL_SLIDESHOW_HH

#include <QLabel>

#include "al_widget.hh"

class ASlideShow: public QLabel
{
Q_OBJECT;
public:
    ASlideShow(QWidget *parent);
    ~ASlideShow();

    void setSource(const QString&);

private slots:
    void showNextSlide();
private:
    QString src;
    QStringList images;
    QStringListIterator *current_img;
    QTimer *tm;
};

class alSlideShow: public alWidgetPre<ASlideShow>
{
Q_OBJECT
public:
    alSlideShow(const QString& id, const QString& parent);
    void setAttr(const QString& name,const QString& value);
};

#endif
