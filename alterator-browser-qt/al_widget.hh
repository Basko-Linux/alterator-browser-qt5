#ifndef QTBROWSER_AL_WIDGET_HH
#define QTBROWSER_AL_WIDGET_HH

#include <QObject>
#include <QLayout>
#include <QListWidgetItem>
#include <QTreeWidgetItem>

#include "utils.hh"
#include "browser.hh"

//Note: I need QObject inheritanse for correct deffered object deletion
// cause I need to destruct objects from it's callbacks
class alWidget: public QObject
{
	Q_OBJECT
protected:
	QString id_;
	QString parent_;
public:

	alWidget(const QString& id,const QString& parent);
	virtual ~alWidget();

	virtual void setAttr(const QString& name,const QString& value);
	virtual void registerEvent(const QString&) {}
	virtual QString postData() const { return ""; }

	virtual QWidget *getWidget(void) = 0;
	virtual QLayout *getViewLayout(void) = 0;
	virtual QWidget *getViewWidget(void) = 0;
	virtual QString getParentId(void) { return parent_; };
	virtual QString getId(void) { return id_; };
public slots:
	void onClick() { emitEvent(id_,"clicked"); }
	void onClick(bool) { emitEvent(id_,"clicked"); }
	void onClick(QListWidgetItem*) { emitEvent(id_,"clicked"); }
	void onClick(QTreeWidgetItem*,int) { emitEvent(id_,"clicked"); }

	void onChange(int) { emitEvent(id_,"changed"); }
	void onChange(void) { emitEvent(id_,"changed"); }
	void onChange(const QString&) { emitEvent(id_,"changed"); }
	void onChange(QWidget*) { emitEvent(id_,"changed"); }

	void onReturn() { emitEvent(id_,"return-pressed"); }
	void onReturn(QListWidgetItem*) { emitEvent(id_,"return-pressed"); }
	void onReturn(QTreeWidgetItem*, int) { emitEvent(id_,"return-pressed"); }

	void onSelect() { emitEvent(id_,"selected"); }
	void onSelect(int) { emitEvent(id_,"selected"); }
	void onSelect(QTreeWidgetItem*,QTreeWidgetItem*) { emitEvent(id_,"selected"); }

	void onDoubleClick() { emitEvent(id_,"double-clicked"); }
	void onDoubleClick(QTreeWidgetItem*,int) { emitEvent(id_,"double-clicked"); }

	void onToggle(bool) { emitEvent(id_,"toggled"); }
	void onToggle(int) { emitEvent(id_,"toggled"); }

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
};

//all current elements on viewer
extern QMap<QString,alWidget*> elements;

template <typename Widget>
Widget *createWidget(const QString& parent)
{
	return new Widget(elements.contains(parent)?elements[parent]->getViewWidget():0);
}

template <typename Widget>
class alWidgetPre: public alWidget
{
protected:
	Widget *wnd_;
public:
	alWidgetPre(const QString& id,const QString& parent):
		alWidget(id,Utils::reparentTag(parent)),
		wnd_(createWidget<Widget>(parent))
	{
		QWidget *p = wnd_->parentWidget();
		if( p )
		{
		    QLayout *l = p->layout();
		    if( l )
			l->addWidget(wnd_);
		}
	}

	~alWidgetPre() { wnd_->deleteLater(); }
	Widget* getWidget() { return wnd_; }	
	virtual QWidget* getViewWidget() { return wnd_; }
	virtual QLayout* getViewLayout() { return wnd_->layout(); }
};



#endif
