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

#include "al_widget.hh"


extern MainWindow *main_window;
QLayout *findViewLayout(const QString& id);
QWidget* findQWidget(const QString& id);
alWidget* findAlWidget(const QString& id);
alWidget *findAlWidgetByName(const QString& name);
QList<alWidget*> findAlChildren(const QString& id);

#include "al_main_widget.hh"

//widgets
class alLabel: public alWidgetPre<QLabel>
{
public:
	alLabel(const QString& id,const QString& parent):
		alWidgetPre<QLabel>(Label, id,parent)
		{
		    //wnd_->setWordWrap( true );
		    //wnd_->setFrameStyle(QFrame::Sunken|QFrame::StyledPanel);
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
	{
	    connect(wnd_, SIGNAL(stateChanged(int)), this, SLOT(onUpdate(int)));
	}
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
	QString getValue() { return  (wnd_->isChecked()?"true":"false"); }
};

#include "al_listbox.hh"

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

#include "al_tabbox.hh"
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

#include "al_tree.hh"

template <typename Widget>
class alHelpPlacePre: public alWidget
{
protected:
	Widget *wnd_;
public:
	alHelpPlacePre(Type type, const QString& id,const QString& parent):
		alWidget(type, id,parent)
	{
	    wnd_ = main_window;
	}

	~alHelpPlacePre() {}
	Widget* getWidget() { return wnd_; }	
	virtual QWidget* getViewWidget() { return wnd_; }	
	virtual QLayout* getViewLayout() { return wnd_->layout(); }	
	void show(bool) {};
};

class alHelpPlace: public alHelpPlacePre<MainWindow_t>
{
public:
	alHelpPlace(const QString& id,const QString& parent):
		alHelpPlacePre<MainWindow_t>(HelpPlace,id,parent)
	{
	}
protected:
	void setAttr(const QString& name,const QString& value);
};


#include "al_slider.hh"

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
#include "al_wizard_face.hh"
#include "al_simplebox.hh"
#include "al_spinbox.hh"

#endif
