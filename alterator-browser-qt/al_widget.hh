#ifndef QTBROWSER_AL_WIDGET_HH
#define QTBROWSER_AL_WIDGET_HH

#include <QObject>
#include <QLayout>
#include <QListWidgetItem>
#include <QTreeWidgetItem>
#include <QTimer>

#include "connection.hh"
#include "utils.hh"
#include "awidget.hh"

//Note: I need QObject inheritanse for correct deffered object deletion
// cause I need to destruct objects from it's callbacks
class alWidget: public QObject
{
	Q_OBJECT
public:
signals:
	void updated();

protected:
	AlteratorWidgetType type_;
	QString id_;
	QString parent_;
	Qt::Alignment children_alignment;
	QString group_;

public:
	alWidget(AlteratorWidgetType type, const QString& id,const QString& parent);
	~alWidget();

	virtual void setAttr(const QString& name,const QString& value);
	virtual void registerEvent(const QString&) {}
	virtual QString postData() const { return ""; }

	virtual void postAddChild(QWidget* chld, AlteratorWidgetType type, const AlteratorRequestActionAttrs &attr);
	virtual QWidget *getWidget(void) = 0;
	virtual QLayout *getViewLayout(void) = 0;
	virtual QWidget *getViewWidget(void) = 0;
	virtual QString getParentId(void) { return parent_; };
	virtual QString getId(void) { return id_; };
	virtual QString getValue() { return ""; };
	QString getGroup() { return group_; };

	AlteratorWidgetType type() { return type_; };

	virtual void markRequired(bool) {};
	virtual void show(bool) = 0;
	virtual void popUp() {};
	virtual void popDown() {};

	static QSizePolicy adjustSizePolicy(const AlteratorWidgetType, const QSizePolicy, const Qt::Orientation orient, const Qt::Orientation parent_orient);
	void setChildrenAlignment(Qt::Alignment);
	Qt::Alignment childrenAlignment();
	void setWndObject(QObject*);

public slots:
	void onWndDestroyed(QObject*);

	void onEvent(const BrowserEventType, const AlteratorRequestFlags);
	void onEvent(const BrowserEventType);

	void onUpdate();
	void onUpdate(int);

	void onClick();
	void onSpecialClick(const AlteratorRequestFlags);
	void onClick(bool);
	void onClick(QListWidgetItem*);
	void onClick(QTreeWidgetItem*,int);

	void onChange();
	void onChange(int);
	void onChange(const QString&);
	void onChange(QWidget*);

	void onReturn();
	void onReturn(QListWidgetItem*);
	void onReturn(QTreeWidgetItem*, int);

	void onSelect();
	void onSelect(int);
	void onSelect(QTreeWidgetItem*,QTreeWidgetItem*);
	void onSelect(const QString&);

	void onDoubleClick();
	void onDoubleClick(QTreeWidgetItem*,int);
	void onDoubleClick(QListWidgetItem*);

	void onToggle(bool);
	void onToggle(int);

//delayed versions
	void delayedOnSelect()
	{
		QTimer::singleShot(0,this, SLOT(onSelect()));
	}
	void delayedOnSelect(const QString&)
	{
		QTimer::singleShot(0,this, SLOT(onSelect()));
	}
	void delayedOnDoubleClick()
	{
		QTimer::singleShot(0,this, SLOT(onDoubleClick()));
	}
private:
	QObject *o_wnd_;
	bool wnd_destroyed;
};

alWidget* alWidgetCreateWidgetGetParent(const QString& parent);

template <typename Widget>
Widget *alWidgetCreateWidget(const QString& parent = 0, const Qt::Orientation o = Qt::Vertical)
{
    alWidget *aw = alWidgetCreateWidgetGetParent(parent);
    return new Widget((aw)? aw->getViewWidget(): 0, o);
}

template <typename Widget>
class alWidgetPre: public alWidget
{
protected:
	Widget *wnd_;
private:
    void init(Widget *wnd_, const QString& parent, AlteratorWidgetType type, const AlteratorRequestActionAttrs &attr);
public:
	alWidgetPre(const AlteratorRequestActionAttrs &attr, AlteratorWidgetType type, const QString& id,const QString& parent):
		alWidget(type,id,Utils::reparentTag(parent))
	{
	    Qt::Orientation orientation = (Qt::Orientation)0;
	    if( attr.contains("orientation") )
		orientation =  attr.value("orientation").o;
	    wnd_ = alWidgetCreateWidget<Widget>(parent, orientation);
	    setWndObject(wnd_);
	    alWidget *a_parent = alWidgetCreateWidgetGetParent(parent);
	    if( a_parent )
		a_parent->postAddChild(wnd_, type, attr);
	}
	~alWidgetPre() {}

	Widget* getWidget() { return wnd_; }
	virtual QWidget* getViewWidget() { return wnd_; }
	virtual QLayout* getViewLayout() { return wnd_->layout(); }
	void show(bool b) { if(b && wnd_) wnd_->show(); else wnd_->hide(); }
};

#endif
