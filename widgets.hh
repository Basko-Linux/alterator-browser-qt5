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

#define MainWidget_t QFrame

extern MainWindow *main_window;

void emitEvent(const QString& id,const QString& type);
MyBoxLayout *getLayout(const QString& id);
QWidget* getQWidget(const QString& id);

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
	virtual QString stringParent(void) { return parent_; };
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
		QWidget *pwidget = getQWidget(parent);
		if( pwidget )
		{
		    QLayout *playout = pwidget->layout();
		    if( playout ) playout->addWidget(wnd_);
		}
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
		alWidget(id,parent)
	{
		//qDebug("%s parent<%s>", __FUNCTION__, parent.toLatin1().data());
		elements[id] = this;
		wnd_ = new Widget(main_window);
		wnd_->setObjectName("main_widget");
		wnd_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

		//wnd_->setFrameStyle(QFrame::Box|QFrame::Raised);
		//main_window->layout()->addWidget(wnd_);
		main_window->setCentralWidget( wnd_ );
	}

	~alMainWidgetPre() { wnd_->deleteLater(); }
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
		tabbox_(getParentTabBox(parent)),
		idx_(tabbox_?static_cast<QTabWidget*>(tabbox_->getWidget())->addTab(wnd_,""):-1)
	{
		new MyVBoxLayout(wnd_);
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
		new MyVBoxLayout(wnd_);
	}
	void setAttr(const QString& name,const QString& value);
	void start() { wnd_->exec(); }
	void stop()  { wnd_->done(0); }
};

class alMainWidget: public alMainWidgetPre<MainWidget_t>
{
    Q_OBJECT;
public:
	alMainWidget(const QString& id,const QString& parent):
		alMainWidgetPre<MainWidget_t>(id, parent)
	{
	    new MyVBoxLayout(wnd_);
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

#endif
