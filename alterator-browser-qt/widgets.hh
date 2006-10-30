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
#include <QProgressBar>
#include <QTreeWidget>
#include <QTextBrowser>
#include <QSlider>
#include <QSplitter>

#include "connection.hh"
#include "main_window.hh"
#include "browser.hh"

typedef QWidget MainWidget_t;

#define simpleQuote(s) s.replace("\\","\\\\").replace("\"","\\\"")

#include "al_widget.hh"


extern MainWindow *main_window;
extern QString help_source;

QLayout *findViewLayout(const QString& id);
QWidget* findQWidget(const QString& id);
alWidget* findAlWidget(const QString& id);
QString reparentTag(QString parent);
QList<alWidget*> findAlChildren(const QString& id);

#include "al_main_widget.hh"

//widgets
class alLabel: public alWidgetPre<QLabel>
{
public:
	alLabel(const QString& id,const QString& parent):
		alWidgetPre<QLabel>(id,parent)
		{
		    wnd_->setWordWrap( true );
		}
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
	alGroupBox(const QString& id,const QString& parent,const QString& checkable):
		alWidgetPre<QGroupBox>(id,parent)
	{
		new MyVBoxLayout(getViewWidget());
		wnd_->setCheckable("true" == checkable);
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

class alMultiListBox: public alWidgetPre<QTreeWidget2>
{
public:
	alMultiListBox(const QString& id,const QString& parent, int cols):
		alWidgetPre<QTreeWidget2>(id,parent)
	{
		//setings to be compatible with QListView
		wnd_->setColumnCount(cols);
		wnd_->header()->hide();
		wnd_->setSelectionBehavior(QAbstractItemView::SelectRows);
		wnd_->setIndentation(0);
	}
protected:
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

class alComboBox: public alWidgetPre<QComboBox>
{
	int counter_;
public:
	alComboBox(const QString& id,const QString& parent):
		alWidgetPre<QComboBox>(id,parent),
		counter_(0)
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
		alWidgetPre<QFrame>(id,":reparent:"+parent),
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

#include "al_dialog.hh"


class alBox: public alWidgetPre<QFrame>
{
public:
	alBox(const QString& id,const QString& parent,MyBoxLayout::Direction direction):
		alWidgetPre<QFrame>(id,parent)
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
	}
		
protected:
	QWidget *getWidget() { return elements[parent_]->getWidget(); }
	QLayout *getViewLayout(void) { return elements[parent_]->getViewLayout(); }
	QWidget *getViewWidget(void) { return elements[parent_]->getViewWidget(); }
};

class alProgressBar: public alWidgetPre<QProgressBar>
{
public:
    alProgressBar(const QString& id,const QString& parent):
	alWidgetPre<QProgressBar>(id,parent)
    {}
protected:
    void setAttr(const QString& name,const QString& value);
};

class alTree: public alWidgetPre<QTreeWidget>
{
	QString items_;
	QString coords_;
	QStringList coordmap_;//internal coordinates map
	void setItems();
	QTreeWidgetItem *findPosition(QTreeWidgetItem *,QStringList,int);
public:
	alTree(const QString& id,const QString& parent,const QString& columns):
		alWidgetPre<QTreeWidget>(id,parent)
	{
		//setings to be compatible with QListView
		wnd_->setColumnCount(columns.isEmpty()? 1 : columns.toInt());
		wnd_->header()->hide();
		wnd_->setSelectionBehavior(QAbstractItemView::SelectRows);
	}
protected:
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

class alHelpPlace: public alWidgetPre<QTextBrowser>
{
Q_OBJECT
public:
	alHelpPlace(const QString& id,const QString& parent):
		alWidgetPre<QTextBrowser>(id,parent)
	{
		if (!help_source.isEmpty()) wnd_->setSource(help_source);
		connect(wnd_,SIGNAL(anchorClicked(const QUrl&)),
		             SLOT(onAnchor(const QUrl&)));
	}
protected:
	void setAttr(const QString& name,const QString& value);
protected slots:
	void onAnchor(const QUrl& url);
};

class alSlider: public alWidgetPre<QSlider>
{
Q_OBJECT
public:
    alSlider(const QString& id, const QString& parent):
	alWidgetPre<QSlider>(id,parent)
    {
	wnd_->setOrientation( Qt::Horizontal );
	wnd_->setTickPosition( QSlider::TicksBothSides );
    }
    void setAttr(const QString& name,const QString& value);
    void registerEvent(const QString&);
    QString postData() const;
};

class alSplitter: public alWidgetPre<QSplitter>
{
Q_OBJECT
public:
    alSplitter(const QString& id, const QString& parent):
	alWidgetPre<QSplitter>(id,parent)
    {
	wnd_->setOrientation( Qt::Horizontal );
    }
    void setAttr(const QString& name,const QString& value);
};

#endif
