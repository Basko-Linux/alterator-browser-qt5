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
public:
	enum Type {
	    UnknownWidget, WizardFace, MainWidget, Dialog,
	    Label, Button, Radio, Edit, TextBox,
	    HGroupBox, VGroupBox, CheckBox, ListBox, MultiListBox, ComboBox,
	    HTabBox, VTabBox, HTabPage, VTabPage, HBox, VBox, GridBox,
	    ProgressBar, Tree, Slider, Separator, SpinBox,
	    HelpPlace, Proxy, Spacer
	};
signals:
	void updated();

protected:
	Type type_;
	QString id_;
	QString parent_;
	Qt::Alignment children_alignment;

public:
	alWidget(Type type, const QString& id,const QString& parent);
	virtual ~alWidget();

	virtual void setAttr(const QString& name,const QString& value);
	virtual void registerEvent(const QString&) {}
	virtual QString postData() const { return ""; }

	virtual void addChild(QWidget* chld, Type type);
	virtual QWidget *getWidget(void) = 0;
	virtual QLayout *getViewLayout(void) = 0;
	virtual QWidget *getViewWidget(void) = 0;
	virtual QString getParentId(void) { return parent_; };
	virtual QString getId(void) { return id_; };
	virtual QString getValue() { return ""; };

	Type type() { return type_; };
	void destroyLater();

	virtual void markRequired(bool) {};
	virtual void show(bool) = 0;
	static QSizePolicy adjustSizePolicy(const Type, const QSizePolicy, const Qt::Orientation parent_orientation);
	void setChildrenAlignment(Qt::Alignment);
	Qt::Alignment childrenAlignment();

public slots:
	void onUpdate(int);

	void onClick();
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
	alWidgetPre(Type type, const QString& id,const QString& parent):
		alWidget(type,id,Utils::reparentTag(parent)),
		wnd_(createWidget<Widget>(parent))
	{
	    alWidget *ap = 0;
	    if( elements.contains(parent) )
		ap = elements[parent];
	    if( ap )
		ap->addChild(wnd_, type);
	}

	~alWidgetPre() { wnd_->deleteLater(); }
	Widget* getWidget() { return wnd_; }	
	virtual QWidget* getViewWidget() { return wnd_; }
	virtual QLayout* getViewLayout() { return wnd_->layout(); }
	void show(bool b) { if(b && wnd_) wnd_->show(); else wnd_->hide(); };
};

#endif
