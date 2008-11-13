#ifndef QTBROWSER_AL_DATEEDIT_HH
#define QTBROWSER_AL_DATEEDIT_HH

#include <QCalendarWidget>
#include <QDateEdit>

#include "al_widget.hh"

class ADateEdit: public AWidget<QWidget>
{
Q_OBJECT
Q_PROPERTY(QString altgroup READ alteratorGroup WRITE setAlteratorGroup)
public:
    ADateEdit(QWidget *parent, const Qt::Orientation);
    ~ADateEdit();

    void setExpanded(bool);
    void setDate(const QString&);
    QString date();

signals:
    void changed();

private:
    QDateEdit *date_edit;
    QCalendarWidget *calendar;
    QVBoxLayout *lay;
};


class alDateEdit: public alWidgetPre<ADateEdit>
{
public:
    alDateEdit(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
    void setAttr(const QString& name,const QString& value);
    void registerEvent(const QString&);
    QString postData() const;
};

#endif
