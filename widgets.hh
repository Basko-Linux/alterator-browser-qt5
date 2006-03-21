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
#include <QDesktopWidget>

#include "connection.hh"
#include "main_window.hh"
#include "browser.hh"

typedef QWidget MainWidget_t;

extern MainWindow *main_window;


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
	virtual QLayout *getViewLayout(void) = 0;
	virtual QWidget *getViewWidget(void) = 0;
	virtual QString getParentId(void) { return parent_; };
	virtual QString getId(void) { return id_; };
public slots:
	void onClick(bool) { emitEvent(id_,"on-click"); }
	void onClick(QListWidgetItem*) { emitEvent(id_,"on-click"); }
	void onChange(void) { emitEvent(id_,"on-change"); }
	void onChange(const QString&) { emitEvent(id_,"on-change"); }
	void onChange(QWidget*) { emitEvent(id_,"on-change"); }
	void onReturn() { emitEvent(id_,"on-return"); }
	void onReturn(QListWidgetItem*) { emitEvent(id_,"on-return"); }
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

QLayout *findViewLayout(const QString& id);
QWidget* findQWidget(const QString& id);
alWidget* findAlWidget(const QString& id);

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
		alWidget(id,parent),
		wnd_(createWidget<Widget>(parent))
	{
		elements[id] = this;
		alWidget *pa = findAlWidget(parent);
		if( pa )
		{
		    QLayout *playout = pa->getViewLayout();
		    if( playout )
			playout->addWidget(wnd_);
		}
	}

	~alWidgetPre() { wnd_->deleteLater(); }
	Widget* getWidget() { return wnd_; }	
	virtual QWidget* getViewWidget() { return wnd_; }
	virtual QLayout* getViewLayout() { return wnd_->layout(); }
};

template <typename Widget>
class alMainWidgetPre: public alWidget
{
protected:
	Widget *wnd_;
public:
	alMainWidgetPre(const QString& id,const QString& parent):
		alWidget(id,parent)
	{
		elements[id] = this;
		wnd_ = new Widget(main_window);
		wnd_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
		main_window->setCentralWidget( wnd_ );
	}

	~alMainWidgetPre() { wnd_->deleteLater(); }
	Widget* getWidget() { return wnd_; }	
	virtual QWidget* getViewWidget() { return wnd_; }	
	virtual QLayout* getViewLayout() { return wnd_->layout(); }	
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
		new MyVBoxLayout(getViewWidget());
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

class alListBox: public alWidgetPre<QListWidget2>
{
public:
	alListBox(const QString& id,const QString& parent):
		alWidgetPre<QListWidget2>(id,parent)
	{
		wnd_->setAlternatingRowColors(true);
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
		tabbox_(getParentTabBox(parent)),
		idx_(tabbox_?static_cast<QTabWidget*>(tabbox_->getWidget())->addTab(wnd_,""):-1)
	{
		new MyVBoxLayout(getViewWidget());
	}
	void setAttr(const QString& name,const QString& value);
private:
	static
	alTabBox *getParentTabBox(const QString& parent)
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
		new MyVBoxLayout(getViewWidget());
	}
	void setAttr(const QString& name,const QString& value);
	void start() { wnd_->exec(); }
	void stop()  { wnd_->done(0); }
};

class alMainWidget: public alMainWidgetPre<MainWidget_t>
{
public:
	alMainWidget(const QString& id,const QString& parent):
		alMainWidgetPre<MainWidget_t>(id, parent)
	{
	    new MyVBoxLayout(getViewWidget());
	}
	void setAttr(const QString& name,const QString& value);
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

class alProxy: public alWidget
{
	QString parent_;
public:
	alProxy(const QString& id,const QString &parent):
		alWidget(id,parent),
		parent_(parent)
	{
		elements[id] = this;
	}
		
protected:
	QWidget *getWidget() { return elements[parent_]->getWidget(); }
	QLayout *getViewLayout(void) { return elements[parent_]->getViewLayout(); }
	QWidget *getViewWidget(void) { return elements[parent_]->getViewWidget(); }
};

#endif
