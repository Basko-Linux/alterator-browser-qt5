#ifndef QTBROWSER_AL_LISTBOX_HH
#define QTBROWSER_AL_LISTBOX_HH

#include <QListWidget>
#include <QTreeWidget>
#include <QKeyEvent>

#include "al_widget.hh"
//QTreeWidget unable to scroll to active widget until it invisible
class ATreeWidget: public QTreeWidget
{
Q_OBJECT
public:
	ATreeWidget(QWidget *parent=0);
	~ATreeWidget();

	void adjustAllColumnsWidth();

signals:
	void spaceBtnPressed();

protected:
	void keyPressEvent(QKeyEvent * e) ;
	void showEvent(QShowEvent *e);
};

class alMultiListBox: public alWidgetPre<ATreeWidget>
{
public:
	alMultiListBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, int cols);
protected:
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
