#ifndef QTBROWSER_AWIDGET_HH
#define QTBROWSER_AWIDGET_HH

#include <QObject>

template <typename TWidget>
class AWidget: public TWidget
{
public:

    AWidget(QWidget *parent): TWidget(parent) {};
    ~AWidget() {};

    QString alteratorGroup() { return alt_group; };
    void setAlteratorGroup(const QString &str) { alt_group = str; };
    QString alteratorType() { return alt_type; };
    void setAlteratorType(const QString &str) { alt_type = str; };

private:
    QString alt_group;
    QString alt_type;
};

#endif
