#ifndef QTBROWSER_AL_SLIDESHOW_HH
#define QTBROWSER_AL_SLIDESHOW_HH

#include <QLabel>

#include "al_widget.hh"

class ASlideShow: public AWidget<QLabel>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ASlideShow(QWidget *parent, const Qt::Orientation);
    ~ASlideShow();

    void setSource(const QString&);
    void setInterval(int);

private slots:
    void showNextSlide();
private:
    QString src;
    QStringList images;
    QStringListIterator *current_img;
    QTimer *tm;
    int interval;
};

class alSlideShow: public alWidgetPre<ASlideShow>
{
Q_OBJECT
public:
    alSlideShow(const AlteratorRequestActionAttrs &attr, const QString& id, const QString& parent);
    void setAttr(const QString& name,const QString& value);
};

#endif
