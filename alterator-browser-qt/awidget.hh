#ifndef QTBROWSER_AWIDGET_HH
#define QTBROWSER_AWIDGET_HH

#include <QObject>

#include "enums.hh"

template <typename TWidget>
class AWidget: public TWidget
{
public:

    AWidget(QWidget *parent): TWidget(parent)
    {
	reg_events = BrowserEventUnknown;
    };
    ~AWidget() {};

    QString getId() { return id; };

    QString alteratorGroup() { return alt_group; };
    void setAlteratorGroup(const QString &str) { alt_group = str; };
    QString alteratorType() { return alt_type; };
    void setAlteratorType(const QString &str) { alt_type = str; };

    void setEventRegistered(const QString &id_, BrowserEventType e) { id = id_; reg_events |= e; };
    bool eventRegistered(BrowserEventType e) { return reg_events & e; };


private:
    QString alt_group;
    QString alt_type;
    QString id;
    BrowserEventTypes reg_events;
};

#endif
