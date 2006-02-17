#ifndef QTBROWSER_WIDGETS_HH
#define QTBROWSER_WIDGETS_HH

#include "layout.hh"
#include "hacks.hh"

#include <QObject>
#include <QString>
#include <QMap>
#include <QTimer>


//widgets
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QLineEdit>
#include <QTextEdit>
#include <QGroupBox>
#include <QCheckBox>
#include <QHeaderView>
#include <QComboBox>
#include <QTabWidget>

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
	virtual QString getParent(void) { return parent_; };
public slots:
	void onClick(bool) { emitEvent(id_,"on-click"); }
	void onChange(void) { emitEvent(id_,"on-change"); }
	void onChange(const QString&) { emitEvent(id_,"on-change"); }
	void onChange(QWidget*) { emitEvent(id_,"on-change"); }
	void onReturn() { emitEvent(id_,"on-return"); }
	void onSelect() { emitEvent(id_,"on-select"); }
	void onSelect(int) { emitEvent(id_,"on-select"); }
	void onDoubleClick() { emitEvent(id_,"on-double-click"); }
	void onToggle(bool) { emitEvent(id_,"on-toggle"); }
	void onToggle(int) { emitEvent(id_,"on-toggle"); }

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

template <typename Widget>
class alMainWidgetPre: public alWidget
{
protected:
	Widget *wnd_;
public:
	alMainWidgetPre(const QString& id,const QString& parent):
		alWidget(id,parent),
		wnd_(createWidget<Widget>(parent))
	{
		elements[id] = this;
		//MyBoxLayout *playout = getLayout(parent);
		//if (playout) playout->addWidget(wnd_);
	}

//	~alMainWidgetPre() { wnd_->deleteLater(); }
	~alMainWidgetPre() {}
	QWidget *getWidget() { return wnd_; }	
};


//widgets
class alLabel: public alWidgetPre<QLabel>
{
public:
	alLabel(const QString& id,const QString& parent):
		alWidgetPre<QLabel>(id,parent)
		{}
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

class alRadio: public alWidgetPre<QRadioButton>
{
public:
	alRadio(const QString& id,const QString& parent):
		alWidgetPre<QRadioButton>(id,parent) {}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
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

class alTextBox: public alWidgetPre<QTextEdit>
{
public:
	alTextBox(const QString& id,const QString& parent):
		alWidgetPre<QTextEdit>(id,parent) {}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

class alGroupBox: public alWidgetPre<QGroupBox>
{
public:
	alGroupBox(const QString& id,const QString& parent):
		alWidgetPre<QGroupBox>(id,parent)
	{
		new MyVBoxLayout(wnd_);
	}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

class alCheckBox: public alWidgetPre<QCheckBox>
{
public:
	alCheckBox(const QString& id,const QString& parent):
		alWidgetPre<QCheckBox>(id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

class alListBox: public alWidgetPre<QTreeWidget2>
{
public:
	alListBox(const QString& id,const QString& parent):
		alWidgetPre<QTreeWidget2>(id,parent)
	{
		wnd_->setAlternatingRowColors(true);
		//setings to be compatible with QListView
		wnd_->setColumnCount(1);
		wnd_->header()->hide();
		wnd_->setSelectionBehavior(QAbstractItemView::SelectRows);
		wnd_->setIndentation(0);
	}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

class alComboBox: public alWidgetPre<QComboBox>
{
public:
	alComboBox(const QString& id,const QString& parent):
		alWidgetPre<QComboBox>(id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

class alTabBox: public alWidgetPre<QTabWidget>
{
public:
	QString current_;
	alTabBox(const QString& id,const QString& parent):
		alWidgetPre<QTabWidget>(id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
};


class alTabPage: public alWidgetPre<QFrame>
{
	alTabBox  *tabbox_;
	int idx_;
public:
	alTabPage(const QString& id,const QString& parent):
		alWidgetPre<QFrame>(id,""),
		tabbox_(getParent(parent)),
		idx_(tabbox_?static_cast<QTabWidget*>(tabbox_->getWidget())->addTab(wnd_,""):-1)
	{
		new MyVBoxLayout(wnd_);
	}
	void setAttr(const QString& name,const QString& value);
private:
	static
	alTabBox *getParent(const QString& parent)
	{
		if (!elements.contains(parent)) return 0;
		return dynamic_cast<alTabBox*>(elements[parent]);
	}
};

class alDialog: public alWidgetPre<QDialog2>
{
public:
	alDialog(const QString& id,const QString& parent):
		alWidgetPre<QDialog2>(id,parent)
	{
		new MyVBoxLayout(wnd_);
	}
	void setAttr(const QString& name,const QString& value);
	void start() { wnd_->exec(); }
	void stop()  { wnd_->done(0); }
};

class alMainWidget: public alWidgetPre<QWidget>
{
public:
	alMainWidget(const QString& id,const QString& parent):
		alWidgetPre<QWidget>(id, parent)
	{
		new MyVBoxLayout(wnd_);
	}
	void setAttr(const QString& name,const QString& value);
//	void start() { wnd_->show(); QApplication::exec(); }
	void start() { wnd_->show(); }
	void stop()  { QApplication::closeAllWindows(); }
};

class alBox: public alWidgetPre<QFrame2>
{
public:
	alBox(const QString& id,const QString& parent,MyBoxLayout::Direction direction):
		alWidgetPre<QFrame2>(id,parent)
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
