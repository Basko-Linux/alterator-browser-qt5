#ifndef QTBROWSER_AL_TREE_HH
#define QTBROWSER_AL_TREE_HH

#include <QTreeWidget>

#include "al_widget.hh"

class ATreeWidget: public AWidget<QTreeWidget>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
    ATreeWidget(QWidget *parent, const Qt::Orientation);
    ~ATreeWidget();
};

class alTree: public alWidgetPre<ATreeWidget>
{
Q_OBJECT
public:
	alTree(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent,const QString& columns);
	void adjustFirstColumnWidth();
	void adjustAllColumnsWidth();

protected:
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;

private Q_SLOTS:
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
