#ifndef QT_BROWSER_WIDGETLIST_HH
#define QT_BROWSER_WIDGETLIST_HH

#include <QObject>
#include <QString>

#include "al_widget.hh"

class WidgetList: public QObject
{
Q_OBJECT
public:
    WidgetList(QObject *parent);
    ~WidgetList();

    void add(const QString& id, alWidget*);
    void removeFromListById(const QString& id);
    void deleteChildrenById(const QString& id);
    QLayout* findViewLayout(const QString& id);
    bool contains(const QString& id);
    alWidget* alWidgetById(const QString& id);
    QList<alWidget*> alChildrenById(const QString& id);
    alWidget* alWidgetByName(const QString& name);
    QWidget* qWidgetById(const QString& id);
    QString makeRequestData();
    void destroyLater(const QString& id);

template <typename T>
    QList<QWidget*> viewVidgetsByQWidgetType() {
	QList<QWidget*> ret;
	foreach(alWidget *aw, elements) {
	    QWidget *w = aw->getWidget();
	    if( qobject_cast<T>(w) )
	    {
		QWidget *vw = aw->getViewWidget();
		if( vw )
		    ret.append(vw);
	    }
	}
	return ret;
}

private:
    QMap<QString,alWidget*> elements;
};

extern WidgetList *widgetlist;

#endif
