#ifndef QTBROWSER_AL_DIALOG_HH
#define QTBROWSER_AL_DIALOG_HH

#include <QScrollArea>

#include "popup.hh"

#include "widgets.hh"

typedef Popup ADialogBase;

class ADialog: public AWidget<ADialogBase>
{
Q_OBJECT
#include "awidget_properties.hh"
public:
	ADialog(QWidget *parent, const Qt::Orientation);
	~ADialog();
	QWidget* getView();

	void addAction(const QString& key, const QString& name, const QString& pixmap);
	void removeAction(const QString &key);
	void clearActions();
	void setDefaultAction(const QString& key);
	QString currentAction();

Q_SIGNALS:
	void actionSelected();

protected:
#if 0
	void closeEvent(QCloseEvent*);
	void showEvent(QShowEvent*);
	void paintEvent(QPaintEvent*);
	void keyPressEvent(QKeyEvent*);
#endif
	bool eventFilter(QObject*, QEvent*);

private Q_SLOTS:
	void onButtonClicked(QAbstractButton*);
	void onFinish(int);

private:
	QScrollArea *scroll;
	QWidget *view_widget;
	QDialogButtonBox *btnbox;
	QString current_action;
	QMap<QString,QDialogButtonBox::StandardButton> key2btn;
	QMap<QString,QAbstractButton*> buttons;
};


class alDialog: public alWidgetPre<ADialog>
{
public:
	alDialog(const AlteratorRequestActionAttrs &attr, const QString& id,const QString& parent);
	void setAttr(const QString& name,const QString& value);
	void registerEvent(const QString&);
	QString postData() const ;
	void popUp();
	void popDown();
	QWidget* getViewWidget();
	QLayout* getViewLayout();
};

#endif
