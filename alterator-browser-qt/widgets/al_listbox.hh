#ifndef QTBROWSER_AL_LISTBOX_HH
#define QTBROWSER_AL_LISTBOX_HH

#include <QListWidget>
#include <QTreeWidget>
#include <QKeyEvent>

#include "al_widget.hh"
//QTreeWidget unable to scroll to active widget until it invisible
class AMultiListBox: public QTreeWidget
{
Q_OBJECT
public:
	enum ListType { ListBox, MultiListBox, RadioListBox, CheckListBox };
	enum RowType { Row, Header };
	AMultiListBox(QWidget *parent=0);
	~AMultiListBox();

	ListType listType();
	void setListType(ListType);
	void adjustAllColumnsWidth();
	void addRow(QStringList&, RowType row_type = Row);
	void setRows(QStringList&);
	void setHeader(QStringList&);

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
};

class alListBox: public alWidgetPre<AMultiListBox>
{
public:
	alListBox(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent, int cols);
protected:
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
};

#endif
