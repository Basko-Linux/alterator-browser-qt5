#ifndef QTBROWSER_WIDGETS_HH
#define QTBROWSER_WIDGETS_HH

#include "layout.hh"

#include <QObject>
#include <QString>
#include <QMap>
#include <QTimer>


//widgets
#include <QLabel>
#include <QPushButton>
#include <QDialog>
#include <QLineEdit>

#include "connection.hh"

void emitEvent(const QString& id,const QString& type);
MyBoxLayout *getLayout(const QString& parent);

#define simpleQuote(s) s.replace("\\","\\\\").replace("\"","\\\"")

//Note: I need QObject inheritanse for correct deffered object deletion
// cause I need to destruct objects from it's callbacks
class alWidget: public QObject
{
	Q_OBJECT
protected:
	QString id_;
	QString parent_;
public:

	alWidget(const QString& id,const QString& parent):
		id_(id),
		parent_(parent)
		{}
	virtual ~alWidget() {}
	virtual void setAttr(const QString& name,const QString& value);
	virtual void registerEvent(const QString&) {}
	virtual QString postData() const { return ""; }

	virtual QWidget *getWidget(void) = 0;
public slots:
	void onClick(bool) { emitEvent(id_,"on-click"); }
	void onChange(void) { emitEvent(id_,"on-change"); }
	void onChange(const QString&) { emitEvent(id_,"on-change"); }
	void onChange(QWidget*) { emitEvent(id_,"on-change"); }
	void onReturn() { emitEvent(id_,"on-return"); }
	void onSelect() { emitEvent(id_,"on-select"); }
	void onDoubleClick() { emitEvent(id_,"on-double-click"); }
	void onToggle(bool) { emitEvent(id_,"on-toggle"); }
	void onToggle() { emitEvent(id_,"on-toggle"); }

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


extern QMap<QString,alWidget*> elements;

template <typename Widget>
Widget *createWidget(const QString& parent)
{
	return new Widget(elements.contains(parent)?elements[parent]->getWidget():0);
}


template <typename Widget>
class alWidgetPre: public alWidget
{
protected:
	Widget *wnd_;
public:
	alWidgetPre(const QString& id,const QString& parent):
		alWidget(id,parent),
		wnd_(createWidget<Widget>(parent))
	{
		elements[id] = this;
		MyBoxLayout *playout = getLayout(parent);
		if (playout) playout->addWidget(wnd_);
	}

	~alWidgetPre() { wnd_->deleteLater(); }
	QWidget *getWidget() { return wnd_; }	
};


//widgets
class alLabel: public alWidgetPre<QLabel>
{
public:
	alLabel(const QString& id,const QString& parent):
		alWidgetPre<QLabel>(id,parent) {}
	void setAttr(const QString& name,const QString& value);
};

class alButton: public alWidgetPre<QPushButton>
{
public:
	alButton(const QString& id,const QString& parent):
		alWidgetPre<QPushButton>(id,parent) {}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
};

class alEdit: public alWidgetPre<QLineEdit>
{
public:
	alEdit(const QString& id,const QString& parent):
		alWidgetPre<QLineEdit>(id,parent) {}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

class alDialog: public alWidgetPre<QDialog>
{
public:
	alDialog(const QString& id,const QString& parent):
		alWidgetPre<QDialog>(id,parent)
	{
		new MyVBoxLayout(wnd_);
	}
	void setAttr(const QString& name,const QString& value);
	void start() { wnd_->exec(); }
	void stop()  { wnd_->done(0); }
};

class alBox: public alWidgetPre<QLabel>
{
public:
	alBox(const QString& id,const QString& parent,MyBoxLayout::Direction direction):
		alWidgetPre<QLabel>(id,parent)
	{
		new MyBoxLayout(wnd_,direction);
	}
	void setAttr(const QString& name,const QString& value);
};

class alVBox: public alBox
{
public:
	alVBox(const QString& id,const QString& parent):
		alBox(id,parent,MyBoxLayout::vertical)
	{}
};

class alHBox: public alBox
{
public:
	alHBox(const QString& id,const QString& parent):
		alBox(id,parent,MyBoxLayout::horizontal)
	{}
};

#endif
