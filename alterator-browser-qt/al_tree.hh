#ifndef QTBROWSER_AL_TREE_HH
#define QTBROWSER_AL_TREE_HH

#include <QTreeWidget>

#include "al_widget.hh"

class alTree: public alWidgetPre<QTreeWidget>
{
Q_OBJECT;
public:
	alTree(const QString& id,const QString& parent,const QString& columns);
	void adjustFirstColumnWidth();
	void adjustAllColumnsWidth();

protected:
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;

private slots:
	void adjustFirstColumnWidth(QTreeWidgetItem*);

private:
	bool expanded_;
	QString items_;
	QString coords_;
	QStringList coordmap_;//internal coordinates map
	void setItems();
	QTreeWidgetItem *findPosition(QTreeWidgetItem *,QStringList,int);
	void expandOrCollapseAllTree();
};

#endif
