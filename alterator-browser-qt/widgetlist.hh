#ifndef QT_BROWSER_WIDGETLIST_HH
#define QT_BROWSER_WIDGETLIST_HH

#include <QObject>
#include <QString>
#include <QButtonGroup>

#include "al_widget.hh"


class WidgetList: public QObject
{
Q_OBJECT
public:
    WidgetList(QObject *parent);
    ~WidgetList();

    void add(const QString& id, alWidget*);
    void groupAdd(alWidget*);
    void groupRemove(alWidget*);
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
    void destroyAll();

template <typename T>
    QList<QWidget*> viewVidgetsByQWidgetType(AlteratorWidgetType t = WUnknown) {
	QList<QWidget*> ret;
	Q_FOREACH(alWidget *aw, elements) {
	    if( t == WUnknown || t == aw->type() )
	    {
		QWidget *w = aw->getWidget();
		if( qobject_cast<T>(w) )
		{
		    QWidget *vw = aw->getViewWidget();
		    if( vw )
			ret.append(vw);
		}
	    }
	}
	return ret;
}

template <typename T>
    QList<T> aWidgetsByType(AlteratorWidgetType t = WUnknown) {
	QList<T> ret;
	Q_FOREACH(alWidget *aw, elements) {
	    if( t == WUnknown || t == aw->type() )
	    {
		T w = qobject_cast<T>(aw->getWidget());
		if( w )
		    ret.append(w);
	    }
	}
	return ret;
}

private:
    QMap<QString,alWidget*> elements;
    QMultiMap<QString,alWidget*> groups;
    QMap<QString,QButtonGroup*> buttongroups;
};

extern WidgetList *widgetlist;

#endif
