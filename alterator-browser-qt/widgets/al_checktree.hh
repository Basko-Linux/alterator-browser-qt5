#ifndef QTBROWSER_AL_CHECKTREE_HH
#define QTBROWSER_AL_CHECKTREE_HH

#include <QTreeWidget>
#include <QKeyEvent>
#include <QMultiHash>

#include "al_widget.hh"


/** ACheckTree is class of tree view with checked items like this:
    [x] Group_1
     |-[x] Item_1_1
     |-[x] Item_1_2
    [ ] Group_2
     |-[x] Item_2_1
*/

class ACheckTree: public AWidget<QTreeWidget>
{
	Q_OBJECT
#include "awidget_properties.hh"
public:
	ACheckTree(QWidget *parent, const Qt::Orientation = Qt::Horizontal);
	~ACheckTree();

	void addRow(const QStringList &data); // id,parent_id,label1,...
	void setRows(const QStringList &data);
	QStringList getChecked();
	QString current();
	QTreeWidgetItem *lookupItem(const QString& id);

public Q_SLOTS:
	void onChange(QTreeWidgetItem*, int);
	void onSelect();
	void onExpand(QTreeWidgetItem *item);

protected:
	void keyPressEvent(QKeyEvent *e) ;
	void showEvent(QShowEvent *e);

private Q_SLOTS:
	void onChangeDeferred();
	void onSelectDeferred();

private:
	QMultiHash<QString,QTreeWidgetItem*> orphaned;
	QTimer *m_changed_timer;
	QTimer *m_selected_timer;
	bool processed_parents;
	bool processed_children;
};

class alCheckTree: public alWidgetPre<ACheckTree>
{
public:
	alCheckTree(const AlteratorWidgetType awtype, const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
protected:
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
