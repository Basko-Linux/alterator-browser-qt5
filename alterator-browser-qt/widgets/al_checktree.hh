#ifndef QTBROWSER_AL_CHECKTREE_HH
#define QTBROWSER_AL_CHECKTREE_HH

#include <QTreeWidget>
#include <QKeyEvent>

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

	void addRow(QStringList data); // checked,label,name,parent,expanded,...
	void setRows(QStringList data);
	QString getSelected();

Q_SIGNALS:
	void spaceBtnPressed();
	void selected();

protected:
	void keyPressEvent(QKeyEvent * e) ;
	void showEvent(QShowEvent *e);

private:
	QTreeWidgetItem *lookupItem(const QString& name);
	QMap<QString, QStringList> orphaned;
	
};

class alCheckTree: public alWidgetPre<ACheckTree>
{
public:
	alCheckTree(const AlteratorWidgetType awtype, const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, int cols);
protected:
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
