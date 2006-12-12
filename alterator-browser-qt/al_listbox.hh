#ifndef QTBROWSER_AL_LISTBOX_HH
#define QTBROWSER_AL_LISTBOX_HH

#include <QListWidget>
#include <QTreeWidget>
#include <QKeyEvent>

#include "al_widget.hh"
//QListWidget unable to scroll to active widget until it invisible
class AListWidget: public QListWidget
{
Q_OBJECT
public:
	AListWidget(QWidget *parent=0);
	~AListWidget();

signals:
	void spaceBtnPressed();

protected:
	void keyPressEvent(QKeyEvent*);
	void showEvent(QShowEvent*);
};

//QTreeWidget unable to scroll to active widget until it invisible
class ATreeWidget: public QTreeWidget
{
Q_OBJECT
public:
	ATreeWidget(QWidget *parent=0);
	~ATreeWidget();

signals:
	void spaceBtnPressed();

protected:
	void keyPressEvent(QKeyEvent * e) ;
	void showEvent(QShowEvent *e);
};

class alListBox: public alWidgetPre<AListWidget>
{
public:
	alListBox(const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

class alMultiListBox: public alWidgetPre<ATreeWidget>
{
public:
	alMultiListBox(const QString& id,const QString& parent, int cols);
protected:
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
