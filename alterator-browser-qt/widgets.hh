#ifndef QTBROWSER_WIDGETS_HH
#define QTBROWSER_WIDGETS_HH

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


#include "al_widget.hh"


extern MainWindow *main_window;
extern QString help_source;

QLayout *findViewLayout(const QString& id);
QWidget* findQWidget(const QString& id);
alWidget* findAlWidget(const QString& id);
QList<alWidget*> findAlChildren(const QString& id);

#include "al_main_widget.hh"

//widgets
class alLabel: public alWidgetPre<QLabel>
{
public:
	alLabel(const QString& id,const QString& parent):
		alWidgetPre<QLabel>(Label, id,parent)
		{
		    wnd_->setWordWrap( true );
		    wnd_->setFrameStyle(QFrame::Sunken|QFrame::StyledPanel);
		}
	void setAttr(const QString& name,const QString& value);
};

class alButton: public alWidgetPre<QPushButton>
{
public:
	alButton(const QString& id,const QString& parent):
		alWidgetPre<QPushButton>(Button, id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
};

class alRadio: public alWidgetPre<QRadioButton>
{
public:
	alRadio(const QString& id,const QString& parent):
		alWidgetPre<QRadioButton>(Radio,id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#include "al_edit.hh"
#include "al_textbox.hh"
#include "al_groupbox.hh"

class alCheckBox: public alWidgetPre<QCheckBox>
{
public:
	alCheckBox(const QString& id,const QString& parent):
		alWidgetPre<QCheckBox>(CheckBox,id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
	QString getValue() { return  (wnd_->isChecked()?"true":"false"); }
};

class alListBox: public alWidgetPre<QListWidget2>
{
public:
	alListBox(const QString& id,const QString& parent):
		alWidgetPre<QListWidget2>(ListBox,id,parent)
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
		alWidgetPre<QTreeWidget2>(MultiListBox,id,parent)
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
		alWidgetPre<QComboBox>(ComboBox,id,parent),
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
		alWidgetPre<QTabWidget>(TabBox,id,parent)
	{}
	void setAttr(const QString& name,const QString& value);
};


class alTabPage: public alWidgetPre<QFrame>
{
	alTabBox  *tabbox_;
	int idx_;
public:
	alTabPage(const QString& id,const QString& parent):
		alWidgetPre<QFrame>(TabPage,id,":reparent:"+parent),
		tabbox_(getParentTabBox(parent)),
		idx_(tabbox_?static_cast<QTabWidget*>(tabbox_->getWidget())->addTab(wnd_,""):-1)
	{
	    QVBoxLayout *bl = new QVBoxLayout(getViewWidget());
	    bl->setSpacing(5);
	    bl->setMargin(5);
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
#include "al_box.hh"

class alProxy: public alWidget
{
	QString parent_;
public:
	alProxy(const QString& id,const QString &parent):
		alWidget(Proxy,id,parent),
		parent_(parent)
	{}
		
protected:
	QWidget *getWidget() { return elements[parent_]->getWidget(); }
	QLayout *getViewLayout(void) { return elements[parent_]->getViewLayout(); }
	QWidget *getViewWidget(void) { return elements[parent_]->getViewWidget(); }
};

class alProgressBar: public alWidgetPre<QProgressBar>
{
public:
    alProgressBar(const QString& id,const QString& parent):
	alWidgetPre<QProgressBar>(ProgressBar,id,parent)
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
		alWidgetPre<QTreeWidget>(Tree,id,parent)
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
		alWidgetPre<QTextBrowser>(HelpPlace,id,parent)
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
	alWidgetPre<QSlider>(Slider,id,parent)
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
	alWidgetPre<QSplitter>(Splitter,id,parent)
    {
	Qt::Orientation orient = Qt::Horizontal;
	QLayout *l = wnd_->layout();
	if( l )
	{
	    QBoxLayout *bl = qobject_cast<QBoxLayout*>(l);
	    if( bl )
	    {
		if( bl->direction() == QBoxLayout::LeftToRight || bl->direction() == QBoxLayout::RightToLeft )
		    orient = Qt::Vertical;
	    }
	}
	wnd_->setOrientation( orient );
    }
    void setAttr(const QString& name,const QString& value);
};

#include "al_spacer.hh"

#endif
