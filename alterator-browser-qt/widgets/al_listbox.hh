#ifndef QTBROWSER_AL_LISTBOX_HH
#define QTBROWSER_AL_LISTBOX_HH

#include <QTreeWidget>
#include <QKeyEvent>

#include "al_widget.hh"

class ASuperListBoxItem: public QTreeWidgetItem
{
public:
    ASuperListBoxItem(QTreeWidget*);
    ~ASuperListBoxItem();
};

class ASuperListBox: public QTreeWidget
{
Q_OBJECT
public:
	enum ListType { ListBox, MultiListBox, RadioListBox, CheckListBox };
	enum RowType { Row, Header };
	ASuperListBox(QWidget *parent, const Qt::Orientation);
	~ASuperListBox();

	ListType listType();
	void setListType(ListType);
	void adjustAllColumnsWidth();
	void addRow(QStringList&, RowType row_type = Row);
	void setRows(QStringList&);
	void setHeader(QStringList&);

	void removeFromSelectedItemsListOld(QTreeWidgetItem*);

signals:
	void spaceBtnPressed();
	void selected();

protected:
	void keyPressEvent(QKeyEvent * e) ;
	void showEvent(QShowEvent *e);

protected slots:
	void onSelectionChanged();

private:
	ListType list_type;
	QList<QTreeWidgetItem*> selected_items_old;
};

class alListBox: public alWidgetPre<ASuperListBox>
{
public:
	alListBox(const AlteratorWidgetType awtype, const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, int cols);
protected:
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
